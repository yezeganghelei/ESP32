/**
 ****************************************************************************************************
 * @file        gif.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       pictureuntiecode-gifuntiecode Code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "gif.h"
#include "piclib.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const uint16_t _aMaskTbl[16] =
{
    0x0000, 0x0001, 0x0003, 0x0007,
    0x000f, 0x001f, 0x003f, 0x007f,
    0x00ff, 0x01ff, 0x03ff, 0x07ff,
    0x0fff, 0x1fff, 0x3fff, 0x7fff,
};
const uint8_t _aInterlaceOffset[] = {8, 8, 4, 2};
const uint8_t _aInterlaceYPos  [] = {0, 4, 2, 1};

uint8_t g_gif_decoding = 0; /* Tagged GIF is being decoded */

/* Define whether to usemalloc,Here we choose to usemalloc */
#if GIF_USE_MALLOC == 0
gif89a tgif89a;         /* gif89adocument */
FIL f_gfile;            /* gif file */
LZW_INFO tlzw;          /* lzw */
#endif

/**
 * @brief       TestGIFhead
 * @param       filename : File name containing path
 * @retval      Judgment result
 *   @arg       0   , yesGIF89a/87a
 *   @arg       other, NoGIF89a/87a
 */
static uint8_t gif_check_head(FIL *filename)
{
    uint8_t gifversion[6];
    uint32_t readed;
    uint8_t res;
    res = f_read(filename, gifversion, 6, (UINT *)&readed);

    if (res)return 1;

    if ((gifversion[0] != 'G') || (gifversion[1] != 'I') || (gifversion[2] != 'F') ||
        (gifversion[3] != '8') || ((gifversion[4] != '7') && (gifversion[4] != '9')) ||
        (gifversion[5] != 'a'))
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief       WillRGB888Turn toRGB565
 * @param       ctb : RGB888Color array first address
 * @retval      RGB565Color value
 */
static uint16_t gif_getrgb565(uint8_t *ctb)
{
    uint16_t r, g, b;
    r = (ctb[0] >> 3) & 0X1F;
    g = (ctb[1] >> 2) & 0X3F;
    b = (ctb[2] >> 3) & 0X1F;
    return b + (g << 5) + (r << 11);
}

/**
 * @brief       Read the color table
 * @param       filename : File name containing path
 * @param       gif      : GIFinformation
 * @param       numcolors: Color table size
 * @retval      Operation results
 *   @arg       0   , success
 *   @arg       other, mistakecode
 */
static uint8_t gif_readcolortbl(FIL *filename, gif89a *gif, uint16_t numcolors)
{
    uint8_t rgb[3];
    uint16_t t;
    uint8_t res;
    uint32_t readed;

    for (t = 0; t < numcolors; t++)
    {
        res = f_read(filename, rgb, 3, (UINT *)&readed);

        if (res)return 1;   /* Read error */

        gif->colortbl[t] = gif_getrgb565(rgb);
    }

    return 0;
}

/**
 * @brief       Get the logical screen descriptor, image size, etc.
 * @param       filename : File name containing path
 * @param       gif      : GIFinformation
 * @retval      Operation results
 *   @arg       0   , success
 *   @arg       other, mistakecode
 */
uint8_t gif_getinfo(FIL *file, gif89a *gif)
{
    uint32_t readed;
    uint8_t res;
    res = f_read(file, (uint8_t *)&gif->gifLSD, 7, (UINT *)&readed);

    if (res)return 1;

    if (gif->gifLSD.flag & 0x80)    /* Global color table exists */
    {
        gif->numcolors = 2 << (gif->gifLSD.flag & 0x07);    /* Get the color table size */

        if (gif_readcolortbl(file, gif, gif->numcolors))
        {
            return 1;   /* Read error */
        }
    }

    return 0;
}

/**
 * @brief Save the global color table
 * @param gif : GIF information
 * @retval None
 */
static void gif_savegctbl(gif89a *gif)
{
    uint16_t i = 0;

    for (i = 0; i < 256; i++)
    {
        gif->bkpcolortbl[i] = gif->colortbl[i]; /* Save global colors */
    }
}

/**
 * @brief       Restore global color table
 * @param       gif      : GIFinformation
 * @retval      none
 */
static void gif_recovergctbl(gif89a *gif)
{
    uint16_t i = 0;

    for (i = 0; i < 256; i++)
    {
        gif->colortbl[i] = gif->bkpcolortbl[i]; /* Restore global color */
    }
}

/**
 * @brief       initializationLZWRelated parameters
 * @param       gif      : GIFinformation
 * @param       codesize : lzwcodelength
 * @retval      none
 */
static void gif_initlzw(gif89a *gif, uint8_t codesize)
{
    memset((uint8_t *)gif->lzw, 0, sizeof(LZW_INFO));
    gif->lzw->SetCodeSize  = codesize;
    gif->lzw->CodeSize     = codesize + 1;
    gif->lzw->ClearCode    = (1 << codesize);
    gif->lzw->EndCode      = (1 << codesize) + 1;
    gif->lzw->MaxCode      = (1 << codesize) + 2;
    gif->lzw->MaxCodeSize  = (1 << codesize) << 1;
    gif->lzw->ReturnClear  = 1;
    gif->lzw->LastByte     = 2;
    gif->lzw->sp           = gif->lzw->aDecompBuffer;
}

/**
 * @brief       Read a block of data
 * @param       filename : File name containing path
 * @param       buf      : Data cache area
 * @param       maxnum   : Maximum read and write data limit
 * @retval      none
 */
static uint16_t gif_getdatablock(FIL *filename, uint8_t *buf, uint16_t maxnum)
{
    uint8_t cnt;
    uint32_t readed;
    uint32_t fpos;
    f_read(filename, &cnt, 1, (UINT *)&readed); /* Get LZW length */

    if (cnt)
    {
        if (buf)    /* Need to read */
        {
            if (cnt > maxnum)
            {
                fpos = f_tell(filename);
                f_lseek(filename, fpos + cnt);  /* jump over */
                return cnt;                     /* Don't read it directly */
            }

            f_read(filename, buf, cnt, (UINT *)&readed);    /* Get LZW length */
        }
        else    /* Skip directly */
        {
            fpos = f_tell(filename);
            f_lseek(filename, fpos + cnt);      /* jump over */
        }
    }

    return cnt;
}

/**
 * @brief       Read an extension block
 *   @note      An extension block can be composed of multiple data blocks, If unknown extension block is found, The read failed
 * @param       filename    : File name containing path
 * @param       gif         : GIFinformation
 * @param       pTransIndex : Transparent color table
 * @param       pDisposal   : How to deal with it
 * @retval      Operation results
 *   @arg       0   , success
 *   @arg       other, mistakecode
 */
static uint8_t gif_readextension(FIL *filename, gif89a *gif, int *pTransIndex, uint8_t *pDisposal)
{
    uint8_t temp;
    uint32_t readed;
    uint8_t buf[4];
    f_read(filename, &temp, 1, (UINT *)&readed);            /* getlength */

    switch (temp)
    {
        case GIF_PLAINTEXT:
        case GIF_APPLICATION:
        case GIF_COMMENT:
            while (gif_getdatablock(filename, 0, 256) > 0); /* Get data blocks */

            return 0;

        case GIF_GRAPHICCTL:    /* Graphics Control Extended Block */
            if (gif_getdatablock(filename, buf, 4) != 4)return 1;   /* Graphics Control Extended BlockoflengthMust be4 */

            gif->delay = (buf[2] << 8) | buf[1];    /* getDelayhour */
            *pDisposal = (buf[0] >> 2) & 0x7;       /* getHow to deal with it */

            if ((buf[0] & 0x1) != 0)*pTransIndex = buf[3];  /* Transparent color table */

            f_read(filename, &temp, 1, (UINT *)&readed);    /* Get LZW length */

            if (temp != 0)return 1; /* Reading data block ending character error */

            return 0;
    }

    return 1;   /* Error data */
}

/**
 * @brief       fromLZWIn cachegetNextLZWcode,Each code contains12Bit
 * @param       filename : File name containing path
 * @param       gif      : GIFinformation
 * @retval      Operation results
 */
static int gif_getnextcode(FIL *filename, gif89a *gif)
{
    int i, j, End;
    long Result;

    if (gif->lzw->ReturnClear)
    {
        /* The first code should be a clearcode. */
        gif->lzw->ReturnClear = 0;
        return gif->lzw->ClearCode;
    }

    End = gif->lzw->CurBit + gif->lzw->CodeSize;

    if (End >= gif->lzw->LastBit)
    {
        int Count;

        if (gif->lzw->GetDone)return -1; /* Error */

        gif->lzw->aBuffer[0] = gif->lzw->aBuffer[gif->lzw->LastByte - 2];
        gif->lzw->aBuffer[1] = gif->lzw->aBuffer[gif->lzw->LastByte - 1];

        if ((Count = gif_getdatablock(filename, &gif->lzw->aBuffer[2], 300)) == 0)gif->lzw->GetDone = 1;

        if (Count < 0)return -1;    /* Error */

        gif->lzw->LastByte = 2 + Count;
        gif->lzw->CurBit = (gif->lzw->CurBit - gif->lzw->LastBit) + 16;
        gif->lzw->LastBit = (2 + Count) * 8;
        End = gif->lzw->CurBit + gif->lzw->CodeSize;
    }

    j = End >> 3;
    i = gif->lzw->CurBit >> 3;

    if (i == j)Result = (long)gif->lzw->aBuffer[i];
    else if (i + 1 == j)Result = (long)gif->lzw->aBuffer[i] | ((long)gif->lzw->aBuffer[i + 1] << 8);
    else Result = (long)gif->lzw->aBuffer[i] | ((long)gif->lzw->aBuffer[i + 1] << 8) | ((long)gif->lzw->aBuffer[i + 2] << 16);

    Result = (Result >> (gif->lzw->CurBit & 0x7))&_aMaskTbl[gif->lzw->CodeSize];
    gif->lzw->CurBit += gif->lzw->CodeSize;
    return (int)Result;
}

/**
 * @brief       getLZWNextcode
 * @param       filename : File name containing path
 * @param       gif      : GIFinformation
 * @retval      Operation results
 *   @arg       >=0 , success
 *   @arg       -1  , unsuccessful
 *   @arg       -2  , Read the ending sign
 */
static int gif_getnextbyte(FIL *filename, gif89a *gif)
{
    int i, Code, Incode;

    while ((Code = gif_getnextcode(filename, gif)) >= 0)
    {
        if (Code == gif->lzw->ClearCode)
        {
            /* Corrupt GIFs can make this happen */
            if (gif->lzw->ClearCode >= (1 << MAX_NUM_LWZ_BITS))return -1; /* Error */

            /* Clear the tables */
            memset((uint8_t *)gif->lzw->aCode, 0, sizeof(gif->lzw->aCode));

            for (i = 0; i < gif->lzw->ClearCode; ++i)gif->lzw->aPrefix[i] = i;

            /* Calculate the'special codes' independence of the initial code size */
            /* and initialize the stack pointer */
            gif->lzw->CodeSize = gif->lzw->SetCodeSize + 1;
            gif->lzw->MaxCodeSize = gif->lzw->ClearCode << 1;
            gif->lzw->MaxCode = gif->lzw->ClearCode + 2;
            gif->lzw->sp = gif->lzw->aDecompBuffer;

            /* Read the first code from the stack after clear ingand initializing */
            do
            {
                gif->lzw->FirstCode = gif_getnextcode(filename, gif);
            } while (gif->lzw->FirstCode == gif->lzw->ClearCode);

            gif->lzw->OldCode = gif->lzw->FirstCode;
            return gif->lzw->FirstCode;
        }

        if (Code == gif->lzw->EndCode)return -2; /* End code */

        Incode = Code;

        if (Code >= gif->lzw->MaxCode)
        {
            *(gif->lzw->sp)++ = gif->lzw->FirstCode;
            Code = gif->lzw->OldCode;
        }

        while (Code >= gif->lzw->ClearCode)
        {
            *(gif->lzw->sp)++ = gif->lzw->aPrefix[Code];

            if (Code == gif->lzw->aCode[Code])return Code;

            if ((gif->lzw->sp - gif->lzw->aDecompBuffer) >= sizeof(gif->lzw->aDecompBuffer))return Code;

            Code = gif->lzw->aCode[Code];
        }

        *(gif->lzw->sp)++ = gif->lzw->FirstCode = gif->lzw->aPrefix[Code];

        if ((Code = gif->lzw->MaxCode) < (1 << MAX_NUM_LWZ_BITS))
        {
            gif->lzw->aCode[Code] = gif->lzw->OldCode;
            gif->lzw->aPrefix[Code] = gif->lzw->FirstCode;
            ++gif->lzw->MaxCode;

            if ((gif->lzw->MaxCode >= gif->lzw->MaxCodeSize) && (gif->lzw->MaxCodeSize < (1 << MAX_NUM_LWZ_BITS)))
            {
                gif->lzw->MaxCodeSize <<= 1;
                ++gif->lzw->CodeSize;
            }
        }

        gif->lzw->OldCode = Incode;

        if (gif->lzw->sp > gif->lzw->aDecompBuffer)return *--(gif->lzw->sp);
    }

    return Code;
}

/**
 * @brief       showGIFimage
 * @param       filename    : File name containing path
 * @param       gif         : GIFinformation
 * @param       x0, y0      : showBitSet
 * @param       Transparency: Transparency index
 * @param       Disposal    : How to deal with it, Include the previous oneimageofHow to deal with it
 *                            whenDisposal = 2hour, Represents transparent pixels, Render with background color
 * @retval      Operation results
 *   @arg       0 , success
 *   @arg       1 , fail
 */
static uint8_t gif_dispimage(FIL *filename, gif89a *gif, uint16_t x0, uint16_t y0, int Transparency, uint8_t Disposal)
{
    uint32_t readed;
    uint8_t lzwlen;
    int Index, OldIndex, XPos, YPos, YCnt, Pass, Interlace, XEnd;
    int Width, Height, Cnt, ColorIndex;
    uint16_t bkcolor;
    uint16_t *pTrans;

    Width = gif->gifISD.width;
    Height = gif->gifISD.height;
    XEnd = Width + x0 - 1;
    bkcolor = gif->colortbl[gif->gifLSD.bkcindex];
    pTrans = (uint16_t *)gif->colortbl;
    f_read(filename, &lzwlen, 1, (UINT *)&readed);  /* Get LZW length */
    gif_initlzw(gif, lzwlen);               /* Initialize the LZW stack with the LZW code size */
    Interlace = gif->gifISD.flag & 0x40;    /* Whether to interweave code */

    for (YCnt = 0, YPos = y0, Pass = 0; YCnt < Height; YCnt++)
    {
        Cnt = 0;
        OldIndex = -1;

        for (XPos = x0; XPos <= XEnd; XPos++)
        {
            if (gif->lzw->sp > gif->lzw->aDecompBuffer)
            {
                Index = *--(gif->lzw->sp);
            }
            else
            {
                Index = gif_getnextbyte(filename, gif);
            }
            
            if (Index == -2)return 0;   /* Endcode */

            if ((Index < 0) || (Index >= gif->numcolors))
            {
                /* IfIndex out of legal range stop decompressing */
                return 1;   /* Error */
            }

            /* If current index equals old index increment counter */
            if ((Index == OldIndex) && (XPos <= XEnd))
            {
                Cnt++;
            }
            else
            {
                if (Cnt)
                {
                    if (OldIndex != Transparency)
                    {
                        pic_phy.draw_hline(XPos - Cnt - 1, YPos, Cnt + 1, *(pTrans + OldIndex));
                    }
                    else if (Disposal == 2)
                    {
                        pic_phy.draw_hline(XPos - Cnt - 1, YPos, Cnt + 1, bkcolor);
                    }

                    Cnt = 0;
                }
                else
                {
                    if (OldIndex >= 0)
                    {
                        if (OldIndex != Transparency)
                        {
                            pic_phy.draw_point(XPos - 1, YPos, *(pTrans + OldIndex));
                        }
                        else if (Disposal == 2)
                        {
                            pic_phy.draw_point(XPos - 1, YPos, bkcolor);
                        }
                    }
                }
            }

            OldIndex = Index;
        }

        if ((OldIndex != Transparency) || (Disposal == 2))
        {
            if (OldIndex != Transparency)ColorIndex = *(pTrans + OldIndex);
            else ColorIndex = bkcolor;

            if (Cnt)
            {
                pic_phy.draw_hline(XPos - Cnt - 1, YPos, Cnt + 1, ColorIndex);
            }
            else
            {
                pic_phy.draw_point(XEnd, YPos, ColorIndex);
            }
        }

        /* Adjust YPos if image is interlaced */
        if (Interlace)   /* Interwoven code */
        {
            YPos += _aInterlaceOffset[Pass];

            if ((YPos - y0) >= Height)
            {
                ++Pass;
                YPos = _aInterlaceYPos[Pass] + y0;
            }
        }
        else
        {
            YPos++;
        }
    }

    return 0;
}

/**
 * @brief       Revert to background color
 * @param       x, y     : coordinate
 * @param       gif      : GIFinformation
 * @param       pimge    : Image descriptor block information
 * @retval      none
 */
static void gif_clear2bkcolor(uint16_t x, uint16_t y, gif89a *gif, ImageScreenDescriptor pimge)
{
    uint16_t x0, y0, x1, y1;
    uint16_t color = gif->colortbl[gif->gifLSD.bkcindex];

    if (pimge.width == 0 || pimge.height == 0)return; /* No need to clear it,It turns out that there is noimage!! */

    if (gif->gifISD.yoff > pimge.yoff)
    {
        x0 = x + pimge.xoff;
        y0 = y + pimge.yoff;
        x1 = x + pimge.xoff + pimge.width - 1;;
        y1 = y + gif->gifISD.yoff - 1;

        if (x0 < x1 && y0 < y1 && x1 < 320 && y1 < 320)
        {
            pic_phy.fill(x0, y0, x1, y1, color); /* Set xy, the range cannot be too large */
        }
    }

    if (gif->gifISD.xoff > pimge.xoff)
    {
        x0 = x + pimge.xoff;
        y0 = y + pimge.yoff;
        x1 = x + gif->gifISD.xoff - 1;;
        y1 = y + pimge.yoff + pimge.height - 1;

        if (x0 < x1 && y0 < y1 && x1 < 320 && y1 < 320)
        {
            pic_phy.fill(x0, y0, x1, y1, color);
        }
    }

    if ((gif->gifISD.yoff + gif->gifISD.height) < (pimge.yoff + pimge.height))
    {
        x0 = x + pimge.xoff;
        y0 = y + gif->gifISD.yoff + gif->gifISD.height - 1;
        x1 = x + pimge.xoff + pimge.width - 1;;
        y1 = y + pimge.yoff + pimge.height - 1;

        if (x0 < x1 && y0 < y1 && x1 < 320 && y1 < 320)
        {
            pic_phy.fill(x0, y0, x1, y1, color);
        }
    }

    if ((gif->gifISD.xoff + gif->gifISD.width) < (pimge.xoff + pimge.width))
    {
        x0 = x + gif->gifISD.xoff + gif->gifISD.width - 1;
        y0 = y + pimge.yoff;
        x1 = x + pimge.xoff + pimge.width - 1;;
        y1 = y + pimge.yoff + pimge.height - 1;

        if (x0 < x1 && y0 < y1 && x1 < 320 && y1 < 320)
        {
            pic_phy.fill(x0, y0, x1, y1, color);
        }
    }
}

/**
 * @brief       Draw one frame of a GIF image
 * @param       filename : File name containing path(.gif)
 * @param       gif      : GIFinformation
 * @param       x0, y0   : The coordinates to start displaying
 * @param       gif      : GIFinformation
 * @retval      Operation results
 *   @arg       0   , success
 *   @arg       other, mistakecode
 */
static uint8_t gif_drawimage(FIL *filename, gif89a *gif, uint16_t x0, uint16_t y0)
{
    uint32_t readed;
    uint8_t res, temp;
    uint16_t numcolors;
    ImageScreenDescriptor previmg;

    uint8_t Disposal = 0;
    int TransIndex;
    uint8_t Introducer;
    TransIndex = -1;

    do
    {
        res = f_read(filename, &Introducer, 1, (UINT *)&readed);    /* Read a byte */

        if (res)return 1;

        switch (Introducer)
        {
            case GIF_INTRO_IMAGE:/* imagedescribe */
                previmg.xoff = gif->gifISD.xoff;
                previmg.yoff = gif->gifISD.yoff;
                previmg.width = gif->gifISD.width;
                previmg.height = gif->gifISD.height;

                res = f_read(filename, (uint8_t *)&gif->gifISD, 9, (UINT *)&readed);    /* Read a byte */

                if (res)return 1;

                if (gif->gifISD.flag & 0x80)    /* Local color table exists */
                {
                    gif_savegctbl(gif);         /* Save the global color table */
                    numcolors = 2 << (gif->gifISD.flag & 0X07); /* getPartialColor table size */

                    if (gif_readcolortbl(filename, gif, numcolors))return 1;    /* Read error */
                }

                if (Disposal == 2)gif_clear2bkcolor(x0, y0, gif, previmg);

                gif_dispimage(filename, gif, x0 + gif->gifISD.xoff, y0 + gif->gifISD.yoff, TransIndex, Disposal);

                while (1)
                {
                    f_read(filename, &temp, 1, (UINT *)&readed);    /* Read a byte */

                    if (temp == 0)break;

                    readed = f_tell(filename); /* There is still a block */

                    if (f_lseek(filename, readed + temp))break; /* Continue to shift backwards */
                }

                if (temp != 0)return 1; /* Error */

                return 0;

            case GIF_INTRO_TERMINATOR:  /* Get the ending sign */
                return 2;   /* representimageuntiecodeCompleted */

            case GIF_INTRO_EXTENSION:
                /* Read image extension*/
                res = gif_readextension(filename, gif, &TransIndex, &Disposal); /* ReadimageExtended block message */

                if (res)return 1;

                break;

            default:
                return 1;
        }
    } while (Introducer != GIF_INTRO_TERMINATOR);   /* Read the ending sign */

    return 0;
}

/**
 * @brief       Exit the current decoding.
 * @param       none
 * @retval      none
 */
void gif_quit(void)
{
    g_gif_decoding = 0;
}

/**
 * @brief       Decode onegifdocument
 *   @note      This function cannot display sizes of large sizes and given sizesgifpicture!!!
 * @param       filename : File name containing path(.gif)
 * @param       gif      : GIFinformation
 * @param       x, y     : The coordinates to start displaying
 * @param       width    : showwidth
 * @param       height   : Display height
 * @retval      Operation results
 *   @arg       0   , success
 *   @arg       other, mistakecode
 */
uint8_t gif_decode(const char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint8_t res = 0;
    uint16_t dtime = 0; /* Decoding delay */
    gif89a *mygif89a;
    FIL *gfile;
    
#if GIF_USE_MALLOC == 1 /* Define whether to usemalloc,Here we choose to usemalloc */
    gfile = (FIL *)piclib_mem_malloc(sizeof(FIL));

    if (gfile == NULL)res = PIC_MEM_ERR; /* Failed to apply for memory */

    mygif89a = (gif89a *)piclib_mem_malloc(sizeof(gif89a));

    if (mygif89a == NULL)res = PIC_MEM_ERR; /* Failed to apply for memory */

    mygif89a->lzw = (LZW_INFO *)piclib_mem_malloc(sizeof(LZW_INFO));

    if (mygif89a->lzw == NULL)res = PIC_MEM_ERR; /* Failed to apply for memory */

#else
    gfile = &f_gfile;
    mygif89a = &tgif89a;
    mygif89a->lzw = &tlzw;
#endif

    if (res == 0)       /* OK */
    {
        res = f_open(gfile, (TCHAR *)filename, FA_READ);

        if (res == 0)   /* Open the fileok */
        {
            if (gif_check_head(gfile))res = PIC_FORMAT_ERR;

            if (gif_getinfo(gfile, mygif89a))res = PIC_FORMAT_ERR;

            if (mygif89a->gifLSD.width > width || mygif89a->gifLSD.height > height)res = PIC_SIZE_ERR;  /* Too large in size */
            else
            {
                x = (width - mygif89a->gifLSD.width) / 2 + x;
                y = (height - mygif89a->gifLSD.height) / 2 + y;
            }

            g_gif_decoding = 1;

            while (g_gif_decoding && res == 0)  /* Decoding loop */
            {
                res = gif_drawimage(gfile, mygif89a, x, y);  /* showOnepicture */

                if (mygif89a->gifISD.flag & 0x80)gif_recovergctbl(mygif89a);    /* Restore global color table */

                if (mygif89a->delay)
                {
                    dtime = mygif89a->delay; /* Get delay time(oneBit 10ms) */
                }
                else 
                {
                    dtime = 10;     /* Default delay (100ms) */
                }
                
                while (dtime-- && g_gif_decoding)
                {
                    vTaskDelay(10);  /* Delay */
                }
                
                if (res == 2)
                {
                    res = 0;
                    break;
                }
            }
        }

        f_close(gfile);
    }

#if GIF_USE_MALLOC == 1 /* Define whether to usemalloc,Here we choose to usemalloc */
    piclib_mem_free(gfile);
    piclib_mem_free(mygif89a->lzw);
    piclib_mem_free(mygif89a);
#endif
    return res;
}
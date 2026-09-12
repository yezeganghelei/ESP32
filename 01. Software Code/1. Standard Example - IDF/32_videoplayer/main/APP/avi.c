/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file avi.c
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief AVI video format analysis code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "avi.h"

AVI_INFO g_avix;                                      /* aviFile-related information */
char *const AVI_VIDS_FLAG_TBL[2] = {"00dc", "01dc"};  /* Video encoding flag string, 00dc/01dc */
char *const AVI_AUDS_FLAG_TBL[2] = {"00wb", "01wb"};  /* Audio encoding flag string, 00wb/01wb */

/**
 * @brief       aviDecoding initialization
 * @param       buf  : Input buffer
 * @param       size : Buffer size
 * @retval      res
 *    @arg      OK,aviFile parsing successfully
 *    @arg      other,Error code
 */
AVISTATUS avi_init(uint8_t *buf, uint32_t size)
{
    uint16_t offset;
    uint8_t *tbuf;
    AVISTATUS res = AVI_OK;
    AVI_HEADER *aviheader;
    LIST_HEADER *listheader;
    AVIH_HEADER *avihheader;
    STRH_HEADER *strhheader;

    STRF_BMPHEADER *bmpheader;
    STRF_WAVHEADER *wavheader;

    tbuf = buf;
    aviheader = (AVI_HEADER *)buf;
    if (aviheader->RiffID != AVI_RIFF_ID)
    {
        return AVI_RIFF_ERR;        /* RIFF ID error */
    }

    if (aviheader->AviID != AVI_AVI_ID)
    {
        return AVI_AVI_ERR;         /* AVI ID error */
    }

    buf += sizeof(AVI_HEADER);      /* offset */
    listheader = (LIST_HEADER *)(buf);
    if (listheader->ListID != AVI_LIST_ID)
    {
        return AVI_LIST_ERR;        /* LIST ID error */
    }

    if (listheader->ListType != AVI_HDRL_ID)
    {
        return AVI_HDRL_ERR;        /* HDRL ID error */
    }

    buf += sizeof(LIST_HEADER);     /* offset */
    avihheader = (AVIH_HEADER *)(buf);
    if (avihheader->BlockID != AVI_AVIH_ID)
    {
        return AVI_AVIH_ERR;        /* AVIH ID error */
    }

    g_avix.SecPerFrame = avihheader->SecPerFrame;   /* Get frame interval time */
    g_avix.TotalFrame = avihheader->TotalFrame;     /* Get total frame count */
    buf += avihheader->BlockSize + 8;               /* offset */
    listheader = (LIST_HEADER *)(buf);
    if (listheader->ListID != AVI_LIST_ID)
    {
        return AVI_LIST_ERR;        /* LIST ID error */
    }

    if (listheader->ListType != AVI_STRL_ID)
    {
        return AVI_STRL_ERR;        /* STRL ID error */
    }

    strhheader = (STRH_HEADER *)(buf + 12);
    if (strhheader->BlockID != AVI_STRH_ID)
    {
        return AVI_STRH_ERR;        /* STRH ID error */
    }

    if (strhheader->StreamType == AVI_VIDS_STREAM)  /* Video frames ahead */
    {
        if (strhheader->Handler != AVI_FORMAT_MJPG)
        {
            return AVI_FORMAT_ERR;  /* Not MJPG video stream, not supported */
        }

        g_avix.VideoFLAG = AVI_VIDS_FLAG_TBL[0];    /* Video stream tag "00dc" */
        g_avix.AudioFLAG = AVI_AUDS_FLAG_TBL[1];    /* Audio Streaming Tag "01wb" */
        bmpheader = (STRF_BMPHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
        if (bmpheader->BlockID != AVI_STRF_ID)
        {
            return AVI_STRF_ERR;    /* STRF ID error */
        }

        g_avix.Width = bmpheader->bmiHeader.Width;
        g_avix.Height = bmpheader->bmiHeader.Height;
        buf += listheader->BlockSize + 8;       /* offset */
        listheader = (LIST_HEADER *)(buf);
        if (listheader->ListID != AVI_LIST_ID)  /* is a video file that does not contain audio frames */
        {
            g_avix.SampleRate = 0;              /* Audio sampling rate */
            g_avix.Channels = 0;                /* Number of audio channels */
            g_avix.AudioType = 0;               /* Audio format */

        }
        else
        {
            if (listheader->ListType != AVI_STRL_ID)
            {
                return AVI_STRL_ERR;    /* STRL ID error */
            }

            strhheader = (STRH_HEADER *)(buf + 12);
            if (strhheader->BlockID != AVI_STRH_ID)
            {
                return AVI_STRH_ERR;    /* STRH IDmistake */
            }

            if (strhheader->StreamType != AVI_AUDS_STREAM)
            {
                return AVI_FORMAT_ERR;  /* Format error */
            }

            wavheader = (STRF_WAVHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
            if (wavheader->BlockID != AVI_STRF_ID)
            {
                return AVI_STRF_ERR;    /* STRF ID error */
            }

            g_avix.SampleRate = wavheader->SampleRate;      /* Audio sampling rate */
            g_avix.Channels = wavheader->Channels;          /* Number of audio channels */
            g_avix.AudioType = wavheader->FormatTag;        /* Audio format */
        }
    }
    else if (strhheader->StreamType == AVI_AUDS_STREAM)     /* Audio frames ahead */
    { 
        g_avix.VideoFLAG = AVI_VIDS_FLAG_TBL[1];            /* Video stream tag "01dc" */
        g_avix.AudioFLAG = AVI_AUDS_FLAG_TBL[0];            /* Audio Streaming Tag "00wb" */
        wavheader = (STRF_WAVHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
        if (wavheader->BlockID != AVI_STRF_ID)
        {
            return AVI_STRF_ERR;                            /* STRF IDmistake */
        }
 
        g_avix.SampleRate = wavheader->SampleRate;          /* Audio sampling rate */
        g_avix.Channels = wavheader->Channels;              /* Number of audio channels */
        g_avix.AudioType = wavheader->FormatTag;            /* Audio format */
        buf += listheader->BlockSize + 8;                   /* offset */
        listheader = (LIST_HEADER *)(buf);
        if (listheader->ListID != AVI_LIST_ID)
        {
            return AVI_LIST_ERR;    /* LIST IDmistake */
        }

        if (listheader->ListType != AVI_STRL_ID)
        {
            return AVI_STRL_ERR;    /* STRL ID error */
        }

        strhheader = (STRH_HEADER *)(buf + 12);
        if (strhheader->BlockID != AVI_STRH_ID)
        {
            return AVI_STRH_ERR;    /* STRH IDmistake */
        }

        if (strhheader->StreamType != AVI_VIDS_STREAM)
        {
            return AVI_FORMAT_ERR;  /* Format error */
        }

        bmpheader = (STRF_BMPHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
        if (bmpheader->BlockID != AVI_STRF_ID)
        {
            return AVI_STRF_ERR;    /* STRF ID error */
        }

        if (bmpheader->bmiHeader.Compression != AVI_FORMAT_MJPG)
        {
            return AVI_FORMAT_ERR;  /* Format error */
        }

        g_avix.Width = bmpheader->bmiHeader.Width;
        g_avix.Height = bmpheader->bmiHeader.Height;
    }

    offset = avi_srarch_id(tbuf, size, "movi");     /* Find movi ID */
    if (offset == 0)
    {
        return AVI_MOVI_ERR;        /* MOVI ID error */
    }

    if (g_avix.SampleRate)          /* Audio stream present, so search for it */
    {
        tbuf += offset;
        offset = avi_srarch_id(tbuf, size, g_avix.AudioFLAG);   /* Find audio stream tag */
        if (offset == 0)
        {
            return AVI_STREAM_ERR;  /* Stream error */
        }
        tbuf += offset + 4;
        g_avix.AudioBufSize = *((uint16_t *)tbuf);              /* Get the audio stream buf size. */
    }

    printf("avi init ok\r\n");
    printf("g_avix.SecPerFrame:%ld\r\n", g_avix.SecPerFrame);
    printf("g_avix.TotalFrame:%ld\r\n", g_avix.TotalFrame);
    printf("g_avix.Width:%ld\r\n", g_avix.Width);
    printf("g_avix.Height:%ld\r\n", g_avix.Height);
    printf("g_avix.AudioType:%d\r\n", g_avix.AudioType);
    printf("g_avix.SampleRate:%ld\r\n", g_avix.SampleRate);
    printf("g_avix.Channels:%d\r\n", g_avix.Channels);
    printf("g_avix.AudioBufSize:%d\r\n", g_avix.AudioBufSize);
    printf("g_avix.VideoFLAG:%s\r\n", g_avix.VideoFLAG);
    printf("g_avix.AudioFLAG:%s\r\n", g_avix.AudioFLAG);

    return res;
}

/**
 * @brief       Find ID
 * @param       buf  : Input buffer
 * @param       size : Buffer size
 * @param       id   : ID to find, must be 4 bytes long
 * @retval      Execution result
 *   @arg       0     , not found
 *   @arg       other  , movi IDOffset
 */
uint32_t avi_srarch_id(uint8_t *buf, uint32_t size, char *id)
{
    uint32_t i;
    uint32_t idsize = 0;
    size -= 4;
    for (i = 0; i < size; i++)
    {
        if ((buf[i] == id[0]) &&
            (buf[i + 1] == id[1]) &&
            (buf[i + 2] == id[2]) &&
            (buf[i + 3] == id[3]))
        {
            idsize = MAKEDWORD(buf + i + 4);    /* Get frame size; must be greater than 16 bytes to return, otherwise it is not valid data */

            if (idsize > 0X10)return i;         /* Find the location of "id" */
        }
    }

    return 0;
}

/**
 * @brief       getstreamflow information
 * @param       buf  : Stream start address(Must be01wb/00wb/01dc/00dcbeginning)
 * @retval      Execution result
 *   @arg       AVI_OK, AVIFile parsing successfully
 *   @arg       other  , Error code
 */
AVISTATUS avi_get_streaminfo(uint8_t *buf)
{
    g_avix.StreamID = MAKEWORD(buf + 2);    /* Get stream type */
    g_avix.StreamSize = MAKEDWORD(buf + 4); /* Get stream size */

    if (g_avix.StreamSize > AVI_MAX_FRAME_SIZE)   /* The frame size is too large, and the error is returned directly */
    {
        printf("FRAME SIZE OVER:%ld\r\n", g_avix.StreamSize);
        g_avix.StreamSize = 0;
        return AVI_STREAM_ERR;
    }
    
    if (g_avix.StreamSize % 2)
    {
        g_avix.StreamSize++;    /* Add odd numbers1(g_avix.StreamSize,Must be even) */
    }

    if (g_avix.StreamID == AVI_VIDS_FLAG || g_avix.StreamID == AVI_AUDS_FLAG)
    {
        return AVI_OK;
    }

    return AVI_STREAM_ERR;
}
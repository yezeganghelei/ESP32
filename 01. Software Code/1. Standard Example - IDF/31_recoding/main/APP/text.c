/**
 ****************************************************************************************************
 * @file        text.c
 * @author      
 * @version     V1.0
 * @date        2020-12-01
 * @brief       Chinese character display Code
 *              supplytext_show_fontandtext_show_stringtwo functions,Used to display Chinese characters
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "text.h"

/**
 * @brief       Obtain Chinese character dot matrix data
 * @param       code  : Current Chinese character encoding(GBKcode)
 * @param       mat   : 当前Chinese character点阵数据存放地址
 * @param       size  : Font size
 *   @note      sizeSize fonts,The size of the dot matrix is: (size / 8 + ((size % 8) ? 1 : 0)) * (size)  byte
 * @retval      none
 */
static void text_get_hz_mat(unsigned char *code, unsigned char *mat, uint8_t size)
{
    unsigned char qh, ql;
    unsigned char i;
    unsigned long foffset;
    uint8_t csize;
    
    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size);             /* 计算字体一个character对应点阵集所占的byte数 */
    qh = *code;
    ql = *(++code);
    if ((qh < 0x81) || (ql < 0x40) || (ql == 0xFF) || (qh == 0xFF)) /* Very common Chinese characters */
    {
        for (i = 0; i < csize; i++)
        {
            *mat++ = 0x00;                                          /* Fill in full form */
        }
        return;
    }
    
    if (ql < 0x7F)
    {
        ql -= 0x40;
    }
    else
    {
        ql -= 0x41;
    }
    
    qh -= 0x81;
    foffset = ((unsigned long)190 * qh + ql) * csize;               /* Get the byte offset in the font library */
    
    switch (size)
    {
        case 12:
        {
            fonts_partition_read(mat, foffset + ftinfo.f12addr, csize);
            break;
        }
        case 16:
        {
            fonts_partition_read(mat, foffset + ftinfo.f16addr, csize);
            break;
        }
        case 24:
        {
            fonts_partition_read(mat, foffset + ftinfo.f24addr, csize);
            break;
        }
    }
}

/**
 * @brief       show一个指定大小的Chinese character
 * @param       x,y   : Coordinates of Chinese characters
 * @param       font  : Chinese characterGBKcode
 * @param       size  : Font size
 * @param       mode  : show模式
 *   @note              0, 正常show(不需要show的点,useLCDBackground color fill,Right nowg_back_color)
 *   @note              1, 叠加show(仅show需要show的点, 不需要show的点, No processing)
 * @param       color : Font color
 * @retval      none
 */
void text_show_font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp, t, t1;
    uint16_t y0 = y;
    uint8_t *dzk;
    uint8_t csize;
    uint8_t font_size = size;
    
    csize = (font_size / 8 + ((font_size % 8) ? 1 : 0)) * (font_size);         /* 计算字体一个character对应点阵集所占的byte数 */
    
    if ((font_size != 12) && (font_size != 16) && (font_size != 24))
    {
        return;
    }
    
    dzk = (uint8_t *)malloc(font_size * 5);                    /* Apply for memory */
    
    if (dzk == NULL)
    {
        return;
    }
    
    text_get_hz_mat(font, dzk, font_size);                           /* Obtain the dot matrix data of the corresponding size */
    
    for (t = 0; t < csize; t++)
    {
        temp = dzk[t];                                          /* Get dot matrix data */
        
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
            {
                lcd_draw_pixel(x, y, color);                    /* 画需要show的点 */
            }
            else if (mode == 0)                                 /* If non-overlapping mode，不需要show的点useBackground color fill */
            {
                lcd_draw_pixel(x, y, 0xffff);             /* Fill background color */
            }
            
            temp <<= 1;
            y++;
            if ((y - y0) == font_size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
    
    free(dzk);                                        /* Free memory */
}

/**
 * @brief       在指定位置开始show一个character串
 *   @note      该函数支持自动Line break
 * @param       x,y   : starting coordinates
 * @param       width : show区域宽度
 * @param       height: show区域高度
 * @param       str   : character串
 * @param       size  : Font size
 * @param       mode  : show模式
 *   @note              0, 正常show(不需要show的点,useLCDBackground color fill,Right nowg_back_color)
 *   @note              1, 叠加show(仅show需要show的点, 不需要show的点, No processing)
 * @param       color : Font color
 * @retval      none
 */
void text_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, uint8_t size, uint8_t mode, uint16_t color)
{
    uint16_t x0 = x;
    uint16_t y0 = y;
    uint8_t bHz = 0;                                                /* characters or chinese */
    char in[100];
    char *pout;
    strcpy(in, str);
    convet_utf8_to_gbk(&pout, in, strlen(in));                                /* utf8-gbk */
    uint8_t *pstr = (uint8_t *)pout;                                /* Point tochar*Type string first address */

    while (*pstr != 0)                                              /* Data not ended */
    {
        if (!bHz)
        {
            if (*pstr > 0x80)                                       /* Chinese */
            {
                bHz = 1;                                            /* The mark is Chinese */
            }
            else                                                    /* character */
            {
                if (x > (x0 + width - size / 2))                    /* Line break */
                {
                    y += size;
                    x = x0;
                }
                
                if (y > (y0 + height - size))                       /* Crossing the line */
                {
                    break;
                }
                
                if (*pstr == 13)                                    /* Line break符号 */
                {
                    y += size;
                    x = x0;
                    pstr++;
                }
                else
                {
                    lcd_show_char(x, y, *pstr, size, mode, color);  /* Valid partial writing */
                }
                
                pstr++;
                x += size / 2;                                      /* The width of the English character is half the width of the Chinese character */
            }
        }
        else                                                        /* Chinese */
        {
            bHz = 0;                                                /* 有Chinese character库 */
            
            if (x > (x0 + width - size))                            /* Line break */
            {
                y += size;
                x = x0;
            }
            
            if (y > (y0 + height - size))                           /* Crossing the line */
            {
                break;
            }
            
            text_show_font(x, y, pstr, size, mode, color);          /* Show this Chinese character, hollow */
            pstr += 2;
            x += size;                                              /* 下一个Chinese character偏移 */
        }
    }

    free(pout);
}

/**
 * @brief       在指定宽度的中间showcharacter串
 *   @note      If the character length exceedslen,Usetext_show_string_middleshow
 * @param       x,y   : starting coordinates
 * @param       str   : character串
 * @param       size  : Font size
 * @param       width : show区域宽度
 * @param       color : Font color
 * @retval      none
 */
void text_show_string_middle(uint16_t x, uint16_t y, char *str, uint8_t size, uint16_t width, uint16_t color)
{
    uint16_t strlenth = 0;
    strlenth = strlen((const char *)str);
    strlenth *= size / 2;

    if (strlenth > width) /* More than, 不能居中show */
    {
        text_show_string(x, y, lcd_self.width, lcd_self.height, str, size, 1, color);
    }
    else
    {
        strlenth = (width - strlenth) / 2;
        text_show_string(strlenth + x, y, lcd_self.width, lcd_self.height, str, size, 1, color);
    }
}
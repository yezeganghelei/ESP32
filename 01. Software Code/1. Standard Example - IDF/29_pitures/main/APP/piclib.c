/**
 ****************************************************************************************************
 * @file        piclib.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Image decoding library Code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "piclib.h"

_pic_info picinfo;      /* Picture information */
_pic_phy pic_phy;       /* Picture display physical interface */

/**
 * @brief multi-point filling
 * @param x, y: starting coordinates
 * @param width, height: width and height
 * @param color: color array
 * @retval None
 */
static void piclib_multi_color(uint16_t x, uint16_t y, uint16_t size, uint16_t *color)
{
    static uint8_t Byte[1024];
    int index = 0;
    uint16_t _x1 = x;
    uint16_t _x2 = _x1 + (size - 1);
    uint16_t _y1 = y;
    uint16_t _y2 = _y1;

    lcd_set_window(_x1, _y1, _x2, _y2);

    for (int i = 0;i < size;i++)
    {
        Byte[index++] = (color[i] >> 8) & 0xFF;
        Byte[index++] = color[i] & 0xFF;
    }

    lcd_write_data(Byte, size * 2);
}

/**
 * @brief       Drawing initialization
 *   @note      Before drawing,This function must be called first, Specify related functions
 * @param       none
 * @retval      none
 */
void piclib_init(void)
{
    pic_phy.draw_point = lcd_draw_pixel;    /* Implementation of dot function,onlyGIFneed */
    pic_phy.fill = lcd_fill;                /* Fill function implementation,onlyGIFneed */
    pic_phy.draw_hline = lcd_draw_hline;    /* Line drawing function implementation,onlyGIFneed */
    pic_phy.multicolor = piclib_multi_color;/* Color fill function implementation,JPEG、BMP、PNGneed */

    picinfo.lcdwidth = lcd_self.width;      /* getLCDWidth pixels */
    picinfo.lcdheight = lcd_self.height;    /* getLCDheight pixels */
}

/**
 * @brief       Smart drawing
 *   @note      Pictures are onlyx,yandwidth, heightDisplay within a limited area.

 * @param       filename      : File name containing path(.bmp/.jpg/.jpeg/.gifwait)
 * @param       x, y          : Start coordinates
 * @param       width, height : display area
 * @param       fast          : Enablefastdecoding
 *   @arg                       0, Not enabled
 *   @arg                       1, Enable
 *   @note                      The image size is smaller thanwaitIn liquid crystal resolution,Supports quick decoding
 * @retval      none
 */
uint8_t piclib_ai_load_picfile(char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint8_t	res = 0;/* Return value */
    uint8_t temp;

    if ((x + width) > picinfo.lcdwidth)return PIC_WINDOW_ERR;   /* xThe coordinates are out of range */

    if ((y + height) > picinfo.lcdheight)return PIC_WINDOW_ERR; /* yThe coordinates are out of range */

    /* getShow box size */
    if (width == 0 || height == 0)return PIC_WINDOW_ERR;        /* Window setting error */

    /* File name delivery */
    temp = exfuns_file_type(filename);   /* getFile type */

    switch (temp)
    {
        case T_BMP:
            res = bmp_decode(filename,width, height);           /* Decode BMP */
            break;

        case T_JPG:
        case T_JPEG:
            res = jpeg_decode(filename,width, height);          /* Decode JPG/JPEG */
            break;

        case T_GIF:
            res = gif_decode(filename, x, y, width, height);    /* Decode gif */
            break;

        case T_PNG:
            res = png_decode(filename, width, height);          /* Decode PNG */
            break;

        default:
            res = PIC_FORMAT_ERR;                               /* Non-picture format!!! */
            break;
    }

    return res;
}

/**
 * @brief dynamically allocate memory
 * @param size : The memory size (bytes) to be applied
 * @retval The first address of the memory allocated to
 */
void *piclib_mem_malloc (uint32_t size)
{
    return (void *)malloc(size);
}

/**
 * @brief Free memory
 * @param paddr : memory home address
 * @retval None
 */
void piclib_mem_free (void *paddr)
{
    free(paddr);
}
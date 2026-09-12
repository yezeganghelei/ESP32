/**
 ****************************************************************************************************
 * @file        png.c
 * @author      ALIENTEK Team (ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Image decoding - PNG decoder
 * @license     Copyright (c) 2020-2032, Guangzhou Xingyi Electronic Technology Co., Ltd.
 ****************************************************************************************************
 * @attention
 *
 * Platform: ALIENTEK ESP32-S3 development board
 * Online video: www.yuanzige.com
 * Technical forum: www.openedv.com
 * Company website: www.alientek.com
 * Purchase: openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "png.h"


/**
 * @brief       Store PNG decoded data into the specified storage area
 * @param       pngle   : PNG handle
 * @param       w       : Width
 * @param       h       : Height
 * @retval      None
 */
void png_init(pngle_t *pngle, uint32_t w, uint32_t h)
{
    pngle->imageWidth = w;
    pngle->imageHeight = h;
    pngle->reduction = false;
    pngle->scale_factor = 1.0;

    /* Calculate the reduction */
    if (pngle->screenWidth < pngle->imageWidth || pngle->screenHeight < pngle->imageHeight)
    {
        pngle->reduction = true;
        double factorWidth = (double)pngle->screenWidth / (double)pngle->imageWidth;
        double factorHeight = (double)pngle->screenHeight / (double)pngle->imageHeight;
        pngle->scale_factor = factorWidth;
        if (factorHeight < factorWidth) pngle->scale_factor = factorHeight;
        pngle->imageWidth = pngle->imageWidth * pngle->scale_factor;
        pngle->imageHeight = pngle->imageHeight * pngle->scale_factor;
    }
}

/**
 * @brief       Store PNG decoded data into the specified storage area
 * @param       pngle   : PNG handle
 * @param       x       : X coordinate
 * @param       y       : Y coordinate
 * @param       w       : Width
 * @param       h       : Height
 * @param       rgb     : RGB color value
 * @retval      None
 */
void png_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
    uint32_t _x = x;
    uint32_t _y = y;

    if (pngle->reduction)
    {
        _x = x * pngle->scale_factor;
        _y = y * pngle->scale_factor;
    }

    if (_y < pngle->screenHeight && _x < pngle->screenWidth)
    {
        pngle->pixels[_y][_x] = rgb565(rgba[0], rgba[1], rgba[2]);
    }
}

/**
 * @brief       PNG decode completion callback function
 * @param       pngle   : PNG handle
 * @retval      None
 */
void png_finish(pngle_t *pngle)
{
    ESP_LOGI(__FUNCTION__, "png_finish");
}

/**
 * @brief       Decode a PNG image
 * @param       filename      : File name with path (.bmp/.jpg/.jpeg/.gif/.png, etc.)
 * @param       width, height : Display area
 * @retval      Decoding speed
 */
TickType_t png_decode(const char *filename, int width, int height,lcd_write_cb lcd_cb)
{
    TickType_t startTick, endTick, diffTick;
    startTick = xTaskGetTickCount();
    uint16_t _width = width;
    uint16_t _cols = 0;
    char buf[1024];
    size_t remain = 0;
    uint16_t _height = height;
    uint16_t _rows = 0;
    double display_gamma = 2.2;
    
    /* Open PNG file */
    FIL* fp;
    uint16_t len;
    fp = (FIL *)malloc(sizeof(FIL));                /* Allocate memory */
    f_open(fp, (const TCHAR *)filename, FA_READ);   /* Open file */

    if (fp == NULL)
    {
        ESP_LOGW(__FUNCTION__, "File not found [%s]", filename);
        return 0;
    }

    /* Initialize the decoding library and set the callback functions */
    pngle_t *pngle = pngle_new(width, height);
    pngle_set_init_callback(pngle, png_init);
    pngle_set_draw_callback(pngle, png_draw);
    pngle_set_done_callback(pngle, png_finish);
    pngle_set_display_gamma(pngle, display_gamma);

    while (!f_eof(fp))
    {
        if (remain >= sizeof(buf))
        {
            ESP_LOGE(__FUNCTION__, "Buffer exceeded");
            while(1) vTaskDelay(1);
        }

        f_read(fp,buf + remain,sizeof(buf) - remain, (UINT *)&len);


        int fed = pngle_feed(pngle, buf, remain + len);

        if (fed < 0)
        {
            ESP_LOGE(__FUNCTION__, "ERROR; %s", pngle_error(pngle));
            while(1) vTaskDelay(1);
        }

        remain = remain + len - fed;

        if (remain > 0)
        {
            memmove(buf, buf + fed, remain);
        }
    }

    f_close(fp);

    if (width > pngle->imageWidth)
    {
        _width = pngle->imageWidth;
        _cols = (width - pngle->imageWidth) / 2;
    }

    if (height > pngle->imageHeight)
    {
        _height = pngle->imageHeight;
        _rows = (height - pngle->imageHeight) / 2;
    }

    uint8_t *colors = (uint8_t*)malloc(_height * _width * 2);

    for(unsigned long y = 0; y < _height; y++)
    {
        for(unsigned long x = 0;x < _width; x++)
        {
            colors[2 * x + y * _width * 2] = (uint8_t)(pngle->pixels[y][x] & 0xFF);
            colors[2 * x + 1 + y * _width * 2] = (uint8_t)(pngle->pixels[y][x] >> 8 &0xFF);
        }
    }

    lcd_cb(_width,_height,colors);

    free(fp);
    free(colors);
    pngle_destroy(pngle, width, height);

    endTick = xTaskGetTickCount();
    diffTick = endTick - startTick;
    ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
    return diffTick;
}

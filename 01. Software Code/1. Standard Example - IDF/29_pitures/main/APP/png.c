/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file png.c
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief picture decoding-png decoding code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "png.h"

/**
 * @brief       PNGdecodingData is stored in the designated storage area
 * @param       pngle   : PNGHandle
 * @param       w       : width
 * @param       h       : high
 * @retval      none
 */
void png_init(pngle_t *pngle, uint32_t w, uint32_t h)
{
    pngle->imageWidth = w;
    pngle->imageHeight = h;
    pngle->reduction = false;
    pngle->scale_factor = 1.0;

    /* Calculate the discount */
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
 * @brief       PNGdecodingData is stored in the designated storage area
 * @param       pngle   : PNGHandle
 * @param       x       : xcoordinate
 * @param       y       : ycoordinate
 * @param       w       : width
 * @param       h       : high
 * @param       rgb     : RGBColor value
 * @retval      none
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
 * @brief       PNGDecoding complete callback function
 * @param       pngle   : PNGHandle
 * @retval      none
 */
void png_finish(pngle_t *pngle)
{
    ESP_LOGI(__FUNCTION__, "png_finish");
}

/**
 * @brief       PNGImage decoding
 * @param       filename      : Contains pathdocumentname(.bmp/.jpg/.jpeg/.gif/.pngwait)
 * @param       width, height : Display area
 * @retval      returnBMPDecoding speed
 */
TickType_t png_decode(const char *filename, int width, int height)
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
    
    /* OpenPNGdocument */
    FIL* fp;
    uint16_t len;
    fp = (FIL *)malloc(sizeof(FIL));                /* Apply for memory */
    f_open(fp, (const TCHAR *)filename, FA_READ);   /* Open the file */

    if (fp == NULL)
    {
        ESP_LOGW(__FUNCTION__, "File not found [%s]", filename);
        return 0;
    }

    /* Initialize the decoding library and set the callback function */
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

    uint16_t *colors = (uint16_t*)malloc(sizeof(uint16_t) * _width);

    for(int y = 0; y < _height; y++)
    {
        for(int x = 0;x < _width; x++)
        {
            colors[x] = pngle->pixels[y][x];
        }

        pic_phy.multicolor(_cols, y+_rows, _width, colors);
        vTaskDelay(1);
    }

    free(fp);
    free(colors);
    pngle_destroy(pngle, width, height);

    endTick = xTaskGetTickCount();
    diffTick = endTick - startTick;
    ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
    return diffTick;
}
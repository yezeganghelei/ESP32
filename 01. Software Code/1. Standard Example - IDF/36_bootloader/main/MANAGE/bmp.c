/**
 ****************************************************************************************************
 * @file        bmp.c
 * @author      ALIENTEK Team (ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Image decoding - BMP decoder
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

#include "bmp.h"


/**
 * @brief       Decode a BMP image
 * @param       filename      : File name with path (.bmp/.jpg/.jpeg/.gif/.png, etc.)
 * @param       width, height : Display area
 * @retval      BMP decoding speed
 */
TickType_t bmp_decode(const char *filename, int width, int height,lcd_write_cb lcd_cb)
{
    TickType_t startTick, endTick, diffTick;
    startTick = xTaskGetTickCount();

    /* Open file */
    esp_err_t ret;
    FIL* fp;
    uint16_t br = 0;
    fp = (FIL *)malloc(sizeof(FIL));    /* Allocate memory */

    ret = f_open(fp, (const TCHAR *)filename, FA_READ); /* Open file */

    if (fp == NULL)
    {
        ESP_LOGW(__FUNCTION__, "File not found [%s]", filename);
        return 0;
    }

    /* Read BMP header */
    bmpfile_t *result = (bmpfile_t*)malloc(sizeof(bmpfile_t));
    ret |= f_read(fp,result->header.magic, 2, (UINT *)&br);

    /* Check whether the image is a BMP file */
    if (result->header.magic[0]!='B' || result->header.magic[1] != 'M')
    {
        ESP_LOGW(__FUNCTION__, "File is not BMP");
        free(result);
        f_close(fp);
        return 0;
    }

    /* Read BMP header info: image size, offset, depth, etc. */
    ret |= f_read(fp,&result->header.filesz, 4, (UINT *)&br);
    ret |= f_read(fp,&result->header.creator1, 2, (UINT *)&br);
    ret |= f_read(fp,&result->header.creator2, 2, (UINT *)&br);
    ret |= f_read(fp,&result->header.offset, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.header_sz, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.width, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.height, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.nplanes, 2, (UINT *)&br);
    ret |= f_read(fp,&result->dib.depth, 2, (UINT *)&br);
    ret |= f_read(fp,&result->dib.compress_type, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.bmp_bytesz, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.hres, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.vres, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.ncolors, 4, (UINT *)&br);
    ret |= f_read(fp,&result->dib.nimpcolors, 4, (UINT *)&br);

    /* Check BMP image depth */
    if ((result->dib.depth == 1) && (result->dib.compress_type == 0))
    {
        /* Not implemented yet */
    }
    else if((result->dib.depth == 24) && (result->dib.compress_type == 0))
    {
        /* BMP row padding (if needed) to a 4-byte boundary */
        uint32_t rowSize = (result->dib.width * 3 + 3) & ~3;
        int w = result->dib.width;
        int h = result->dib.height;
        int _x;
        int _w;
        int _cols;
        int _cole;
        int _y;
        int _rows;
        int _rowe;

        if (width >= w)
        {
            _x = (width - w) / 2;
            _w = w;
            _cols = 0;
            _cole = w - 1;
        }
        else
        {
            _x = 0;
            _w = width;
            _cols = (w - width) / 2;
            _cole = _cols + width - 1;
        }

        if (height >= h)
        {
            _y = (height - h) / 2;
            _rows = 0;
            _rowe = h -1;
        }
        else
        {
            _y = 0;
            _rows = (h - height) / 2;
            _rowe = _rows + height - 1;
        }

        uint8_t sdbuffer[3 * 20]; /* Pixel buffer (R+G+B per pixel) */
        uint8_t *colors = (uint8_t*)malloc(h * w * 2);;

        for (int row = 0; row < h; row++)
        {
            
            if (row < _rows || row > _rowe)
            {
                continue;
            }

            /* Seek to the start of the scan line */
            int pos = result->header.offset + (h - 1 - row) * rowSize;
            f_lseek(fp, pos);
            int buffidx = sizeof(sdbuffer); /* Force buffer reload */

            for (int col = 0; col < w; col++)
            {
                if (buffidx >= sizeof(sdbuffer))
                {
                    f_read(fp,sdbuffer, sizeof(sdbuffer), (UINT *)&br);
                    buffidx = 0;
                }

                if (col < _cols || col > _cole)
                {
                    continue;
                }

                /* Convert pixels from BMP to TFT format */
                uint8_t b = sdbuffer[buffidx++];
                uint8_t g = sdbuffer[buffidx++];
                uint8_t r = sdbuffer[buffidx++];
                colors[2 * col + row * w *2] = (uint8_t)(rgb565(r, g, b) & 0xFF);
                colors[2 * col + 1+ row * w *2] =  (uint8_t)(rgb565(r, g, b) >> 8 &0xFF);
            }
            _y++;
        }

        lcd_cb(w,h,colors);

        free(colors);
    }

    free(result);
    f_close(fp);

    endTick = xTaskGetTickCount();
    diffTick = endTick - startTick;
    ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
    return diffTick;
}

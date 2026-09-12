/**
 ****************************************************************************************************
* @file        jpeg.c
* @author      ALIENTEK Team (ALIENTEK)
* @version     V1.0
* @date        2023-12-01
* @brief       Image decoding - JPEG/JPG decoder
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

#include "jpeg.h"


/* The TJPGD outside the ROM code is newer and has a different return type in the decode callback */
typedef int jpeg_decode_out_t;
JDEC jpeg_dev;                  /* Decoder object structure pointer */

/**
 * @brief       JPEG data input callback function
 * @param       jd       : Structure holding the object information to be decoded
 * @param       buf      : Input data buffer (NULL: perform address offset)
 * @param       num      : Number of bytes to read from the input stream / address offset
 * @retval      Number of bytes read / address offset
 */
static unsigned int infunc(JDEC *decoder, uint8_t *buf, unsigned int len)
{
    uint16_t  rb;                           /* Number of bytes read */
    FIL *dev = (FIL *)decoder->device;      /* Information of the file to be decoded, defined using the FATFS FIL structure type */

    if (buf)                                /* Read data is valid, start reading data */
    {
        f_read(dev, buf, len, (UINT *)&rb); /* Call the FATFS f_read function to read data from the JPEG file */
        return rb;                          /* Return the number of bytes read */
    }
    else
    {
        return (f_lseek(dev, f_tell(dev) + len) == FR_OK) ? len : 0;    /* Reposition the data pointer, equivalent to discarding the previous n bytes */
    }
}

/**
 * @brief       Output function. Re-encode RGB888 data from the decoder into big-endian RGB565
 * @param       decoder : JPEG decode structure
 * @param       bitmap  : Bitmap data
 * @param       rect    : Image coordinate information
 * @retval      Returns 1 on successful output
 */
static jpeg_decode_out_t outfunc(JDEC *decoder, void *bitmap, JRECT *rect)
{
    JDEC *jd = (JDEC *) decoder;
    uint8_t *in = (uint8_t *) bitmap;

    for (int y = rect->top; y <= rect->bottom; y++)
    {
        for (int x = rect->left; x <= rect->right; x++)
        {
            if (y < jd->screenHeight && x < jd->screenWidth)
            {
                jd->outData[y][x] = rgb565(in[0], in[1], in[2]);
            }

            in += 3;
        }
    }

    return 1;
}

/**
 * @brief       Specify the output scale factor N. The output image is scaled by 1/2^N (N=0 to 3)
 * @param       screenWidth     : Screen width
 * @param       screenHeight    : Screen height
 * @param       decodeWidth     : Decode width
 * @param       decodeHeight    : Decode height
 * @retval      0: failure; 1: 1/2 scale; 2: 1/4 scale; 3: original size
 */
uint8_t getScale(int screenWidth, int screenHeight, uint16_t decodeWidth, uint16_t decodeHeight)
{
    if (screenWidth >= decodeWidth && screenHeight >= decodeHeight)
    {
        return 0;
    }

    double scaleWidth = (double)decodeWidth / (double)screenWidth;
    double scaleHeight = (double)decodeHeight / (double)screenHeight;
    double scale = scaleWidth;

    if (scaleWidth < scaleHeight)
    {
        scale = scaleHeight;
    }

    if (scale <= 2.0)
    {
        return 1;
    }

    if (scale <= 4.0)
    {
        return 2;
    }

    return 3;
}

/**
 * @brief       JPEG/JPG decoding function
 * @param       pixels          : Pixels
 * @param       file            : File name to read
 * @param       screenWidth     : Screen width
 * @param       screenHeight    : Screen height
 * @param       imageWidth      : Decode width
 * @param       imageHeight     : Decode height
 * @retval      0: failure; 1: 1/2 scale; 2: 1/4 scale; 3: original size
 */
esp_err_t decode_jpeg(pixel_jpeg ***pixels, char * file, int screenWidth, int screenHeight, int * imageWidth, int * imageHeight)
{
    char *work = NULL;
    FIL *f_jpeg = NULL;     /* JPEG file pointer */
    *pixels = NULL;
    JRESULT res = JDR_OK;
    esp_err_t ret = ESP_OK;
    uint32_t jd_work_size = 6144 + 4096;

    /* Allocate pixel memory. Each row is an array of IMAGE_W 16-bit pixels; the "*pixels" array itself contains pointers to these rows */
    *pixels = malloc( sizeof(pixel_jpeg *) * screenHeight);
    /* Allocation failed */
    if (*pixels == NULL)
    {
        ESP_LOGE(__FUNCTION__, "Error allocating memory for lines");
        ret = ESP_ERR_NO_MEM;
        goto err;
    }

    for (int i = 0; i < screenHeight; i++)
    {
        (*pixels)[i] = malloc(screenWidth * sizeof(pixel_jpeg));

        if ((*pixels)[i] == NULL)
        {
            ESP_LOGE(__FUNCTION__, "Error allocating memory for line %d", i);
            ret = ESP_ERR_NO_MEM;
            goto err;
        }
    }

    /* Allocate workspace for the JPEG decoder */
    work = malloc(jd_work_size);

    if (work == NULL)
    {
        ESP_LOGE(__FUNCTION__, "Cannot allocate workspace");
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
    
    /* Fill in the fields of the JpegDev structure */
    jpeg_dev.outData = *pixels;
    jpeg_dev.screenWidth = screenWidth;
    jpeg_dev.screenHeight = screenHeight;

    f_jpeg = (FIL *)malloc(sizeof(FIL));    /* Allocate memory */

    f_open(f_jpeg, (const TCHAR *)file, FA_READ); /* Open file */

    if (f_jpeg == NULL)
    {
        ESP_LOGW(__FUNCTION__, "Image file not found [%s]", file);
        ret = ESP_ERR_NOT_FOUND;
        goto err;
    }

    /* Prepare and decode the JPEG */
    res = jd_prepare(&jpeg_dev, infunc, work, jd_work_size, f_jpeg);

    if (res != JDR_OK)
    {
        ESP_LOGE(__FUNCTION__, "Image decoder: jd_prepare failed (%d)", res);
        ret = ESP_ERR_NOT_SUPPORTED;
        goto err;
    }

    /* Calculate the scale factor */
    uint8_t scale = getScale(screenWidth, screenHeight, jpeg_dev.width, jpeg_dev.height);

    /* Calculate the image size */
    double factor = 1.0;
    if (scale == 1) factor = 0.5;
    if (scale == 2) factor = 0.25;
    if (scale == 3) factor = 0.125;
    *imageWidth = (double)jpeg_dev.width * factor;
    *imageHeight = (double)jpeg_dev.height * factor;
    /* Decompress */
    res = jd_decomp(&jpeg_dev, outfunc, scale);

    if (res != JDR_OK)
    {
        ESP_LOGE(__FUNCTION__, "Image decoder: jd_decode failed (%d)", res);
        ret = ESP_ERR_NOT_SUPPORTED;
        goto err;
    }

    /* All done! Free memory */
    free(work);
    f_close(f_jpeg);
    return ret;

    /* If a decoding error occurs, execute the following code */
    err:
    f_close(f_jpeg);

    if (*pixels != NULL)
    {
        for (int i = 0; i < screenHeight; i++)
        {
            if ((*pixels)[i]) free((*pixels)[i]);
        }

        free(*pixels);
    }

    free(work);
    return ret;
}

/**
 * @brief       Free memory after display is complete
 * @param       pixels          : Pixel address
 * @param       screenWidth     : Screen width
 * @param       screenHeight    : Screen height
 * @retval      ESP_OK: success; others: failure
 */
esp_err_t release_image(pixel_jpeg ***pixels, int screenWidth, int screenHeight)
{
    if (*pixels != NULL)
    {
        for (int i = 0; i < screenHeight; i++)
        {
            free((*pixels)[i]);
        }
        free(*pixels);
    }

    return ESP_OK;
}

/**
 * @brief       Decode a JPEG image
 * @param       filename        : File name with path (.bmp/.jpg/.jpeg/.gif/.png, etc.)
 * @param       width, height   : Display area
 * @param       lcd_cb          : Drawing callback function
 * @retval      Decoding speed
 */
TickType_t jpeg_decode(const char *filename, int width, int height,lcd_write_cb lcd_cb)
{
    TickType_t startTick, endTick, diffTick;
    startTick = xTaskGetTickCount();

    pixel_jpeg **pixels;
    int imageWidth;
    int imageHeight;
    esp_err_t err = decode_jpeg(&pixels, (char *)filename, width, height, &imageWidth, &imageHeight);
    ESP_LOGI(__FUNCTION__, "decode_image err=%d imageWidth=%d imageHeight=%d", err, imageWidth, imageHeight);
    if (err == ESP_OK)
    {

        uint16_t _width = width;
        uint16_t _cols = 0;

        if (width > imageWidth)
        {
            _width = imageWidth;
            _cols = (width - imageWidth) / 2;
        }

        ESP_LOGI(__FUNCTION__, "_width=%d _cols=%d", _width, _cols);

        uint16_t _height = height;
        uint16_t _rows = 0;

        if (height > imageHeight)
        {
            _height = imageHeight;
            _rows = (height - imageHeight) / 2;
        }

        ESP_LOGI(__FUNCTION__, "_height=%d _rows=%d", _height, _rows);
        uint8_t *colors = (uint8_t*)malloc(_height * _width * 2);

        for(unsigned long y = 0; y < _height; y++)
        {
            for(unsigned long x = 0;x < _width; x++)
            {
                colors[2 * x + y * _width * 2] = (uint8_t)(pixels[y][x] & 0xFF);
                colors[2 * x + 1 + y * _width * 2] = (uint8_t)(pixels[y][x] >> 8 &0xFF);
            }
        }

        lcd_cb(_width,_height,colors);
        free(colors);
        release_image(&pixels, width, height);
        ESP_LOGI(__FUNCTION__, "Finish");
    }
    else
    {
        ESP_LOGE(__FUNCTION__, "decode_jpeg fail=%d", err);
    }

    endTick = xTaskGetTickCount();
    diffTick = endTick - startTick;
    ESP_LOGI(__FUNCTION__, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
    return diffTick;
}

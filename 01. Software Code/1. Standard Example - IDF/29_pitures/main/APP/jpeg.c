/**
 ****************************************************************************************************
* @file        jpeg.c
* @author      
* @version     V1.0
* @date        2023-12-01
* @brief       Image decoding-jpeg/jpgdecoding Code
* @license     Copyright (c) 2020-2032, 
****************************************************************************************************
* @attention

****************************************************************************************************
*/

#include "jpeg.h"

/* ROMOutside the codeTJPGDNewer，existdecodingThere are different callsbacksofreturntype */
typedef int jpeg_decode_out_t;
JDEC jpeg_dev;                  /* Pointer to the object structure to be decoded */

/**
 * @brief       jpegData input callbackfunction
 * @param       jd       : Structure of storing object information to be decoded
 * @param       buf      : Input data buffer (NULL:Perform address offset)
 * @param       num      : The amount of data to be read from the input data stream/Address offset
 * @retval      Read the number of bytes/Address offset
 */
static unsigned int infunc(JDEC *decoder, uint8_t *buf, unsigned int len)
{
    uint16_t  rb;                           /* Read the number of bytes */
    FIL *dev = (FIL *)decoder->device;      /* Information about the file to be decoded，useFATFSIn-houseFILDefine structure type */

    if (buf)                                /* Reading data is valid, start reading data */
    {
        f_read(dev, buf, len, (UINT *)&rb); /* CallFATFSoff_readfunction，Used tojpegdocumentofRead out the data */
        return rb;                          /* return reads the arriveof number of bytes */
    }
    else
    {
        return (f_lseek(dev, f_tell(dev) + len) == FR_OK) ? len : 0;    /* Relocate the data points，Equivalent to deletion beforenByte data */
    }
}

/**
 * @brief       Output function。Will be from the decoderRGB888Data recodeforbig-endian RGB565
 * @param       decoder : JPEGDecode structure
 * @param       bitmap  : Bitmap data
 * @param       rect    : Image coordinate information
 * @retval      return1The output is successful
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
 * @brief Specify the scaling factor for the outputN. The output image is scaled for1/2^N (N=0arrive3)
 * @param screenWidth : Screen width
 * @param screenHeight : Screen height
 * @param decodeWidth : Decode width
 * @param decodeHeight : Decoding height
 * @retval 0:fail;1;2Multiply;2:4Multiply;3:Original size
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
 * @brief JPEG/JPGdecodingfunction
 * @param pixels : Pixels point
 * @param file : Read file name
 * @param screenWidth : Screen width
 * @param screenHeight : Screen height
 * @param imageWidth : Decode width
 * @param imageHeight : Decoding height
 * @retval 0:fail;1;2Multiply;2:4Multiply;3:Original size
 */
esp_err_t decode_jpeg(pixel_jpeg ***pixels, char * file, int screenWidth, int screenHeight, int * imageWidth, int * imageHeight)
{
    char *work = NULL;
    FIL *f_jpeg = NULL;     /* JPEG file pointer */
    *pixels = NULL;
    JRESULT res = JDR_OK;
    esp_err_t ret = ESP_OK;
    uint32_t jd_work_size = 6144 + 4096;

    /* AlocatePixelsMemory。Each line isIMAGE_W 16BitPixelsofArray；“*Pixels”The array itself contains pointers to the rows */
    *pixels = malloc( sizeof(pixel_jpeg *) * screenHeight);
    /* Application failed */
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

    /* forjpegDecoder allocates workspace */
    work = malloc(jd_work_size);

    if (work == NULL)
    {
        ESP_LOGE(__FUNCTION__, "Cannot allocate workspace");
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
    
    /* fillingJpegDevFields of structure */
    jpeg_dev.outData = *pixels;
    jpeg_dev.screenWidth = screenWidth;
    jpeg_dev.screenHeight = screenHeight;

    f_jpeg = (FIL *)malloc(sizeof(FIL));    /* Apply for memory */

    f_open(f_jpeg, (const TCHAR *)file, FA_READ); /* Open the file */

    if (f_jpeg == NULL)
    {
        ESP_LOGW(__FUNCTION__, "Image file not found [%s]", file);
        ret = ESP_ERR_NOT_FOUND;
        goto err;
    }

    /* Prepare and decodejpeg */
    res = jd_prepare(&jpeg_dev, infunc, work, jd_work_size, f_jpeg);

    if (res != JDR_OK)
    {
        ESP_LOGE(__FUNCTION__, "Image decoder: jd_prepare failed (%d)", res);
        ret = ESP_ERR_NOT_SUPPORTED;
        goto err;
    }

    /* Calculate the scale factor */
    uint8_t scale = getScale(screenWidth, screenHeight, jpeg_dev.width, jpeg_dev.height);

    /* Calculate image size */
    double factor = 1.0;
    if (scale == 1) factor = 0.5;
    if (scale == 2) factor = 0.25;
    if (scale == 3) factor = 0.125;
    *imageWidth = (double)jpeg_dev.width * factor;
    *imageHeight = (double)jpeg_dev.height * factor;
    /* Decompile */
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
 * @brief       Release memory after display is completed
 * @param       pixels          : Pixelspoint地址
 * @param       screenWidth     : Screen width
 * @param       screenHeight    : Screen height
 * @retval      ESP_OK:success;other:fail
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
 * @brief       JPEGImage decoding
 * @param       filename      : filename containing path(.bmp/.jpg/.jpeg/.gif/.pngwait)
 * @param       width, height : Display area
 * @retval      returnBMPDecoding speed
 */
TickType_t jpeg_decode(const char *filename, int width, int height)
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
        uint16_t *colors = (uint16_t*)malloc(sizeof(uint16_t) * _width);

        for(int y = 0; y < _height; y++)
        {
            for(int x = 0;x < _width; x++)
            {
                colors[x] = pixels[y][x];
            }

            pic_phy.multicolor(_cols, y+_rows, _width, colors);
            vTaskDelay(1);
        }

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
/**
 ****************************************************************************************************
 * @file        image.c
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Image library code (provides image_update_image and images_init for updating and initializing the image library)
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

#include "image.h"
#include "lcd.h"
#include "ff.h"
#include <string.h>

/* Total number of sectors used by the image library area */
#define IMAGESECSIZE         90

/* Image library start address */
#define IMAGEINFOADDR        0

/* Limit each operation to 4K */
#define SECTOR_SIZE          0X1000

/* Stores image library basic information, addresses, sizes, etc. */
_image_info g_ftinfo;

static const char *TAG = "storage_partition";
const esp_partition_t *storage_partition;

/* Paths of the image library on disk */
char *const IMAGE_GBK_PATH[10] =
{
    "/SYSTEM/LVGLBIN/lv_camera.BIN",
    "/SYSTEM/LVGLBIN/lv_file.BIN",
    "/SYSTEM/LVGLBIN/lv_video.BIN",
    "/SYSTEM/LVGLBIN/lv_setting.BIN",
    "/SYSTEM/LVGLBIN/lv_weather.BIN",
    "/SYSTEM/LVGLBIN/lv_measure.BIN",
    "/SYSTEM/LVGLBIN/lv_photo.BIN",
    "/SYSTEM/LVGLBIN/lv_music.BIN",
    "/SYSTEM/LVGLBIN/lv_calendar.BIN",
    "/SYSTEM/LVGLBIN/lv_background.BIN",
};

/* Prompt messages during update */
char *const IMAGE_UPDATE_REMIND_TBL[10] =
{
    "Updating lv_camera.BIN",
    "Updating lv_file.BIN",
    "Updating lv_video.BIN",
    "Updating lv_setting.BIN",
    "Updating lv_weather.BIN",
    "Updating lv_measure.BIN",
    "Updating lv_photo.BIN",
    "Updating lv_music.BIN",
    "Updating lv_calendar.BIN",
    "Updating lv_background.BIN",
};

#define IMAGE_GBK_NUM           (int)(sizeof(IMAGE_GBK_PATH) / sizeof(IMAGE_GBK_PATH[0]))
#define IMAGE_UPDATE_REMIND_NUM (int)(sizeof(IMAGE_UPDATE_REMIND_TBL) / sizeof(IMAGE_UPDATE_REMIND_TBL[0]))

/**
 * @brief       Read data from the partition table
 * @param       buffer    : storage area for the read data
 * @param       offset    : start address of the data to read
 * @param       length    : read size
 * @retval      ESP_OK: success; other: failure
 */
esp_err_t images_partition_read(void *buffer, uint32_t offset, uint32_t length)
{
    esp_err_t err;

    if (buffer == NULL)
    {
        ESP_LOGE(TAG, "ESP_ERR_INVALID_ARG");
        return ESP_ERR_INVALID_ARG;
    }

    err = esp_partition_read(storage_partition, offset, buffer, length);
    
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Flash read failed.");
        return err;
    }
    
    return err;
}

/**
 * @brief       Write data to the partition table
 * @param       buffer    : storage area for the data to write
 * @param       offset    : start address of the data to write
 * @param       length    : write size
 * @retval      ESP_OK: success; other: failure
 */
esp_err_t images_partition_write(void *buffer, uint32_t offset, uint32_t length)
{
    esp_err_t err;

    if (buffer == NULL)
    {
        ESP_LOGE(TAG, "ESP_ERR_INVALID_ARG");
        return ESP_ERR_INVALID_ARG;
    }

    err = esp_partition_write(storage_partition, offset, buffer, length);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Flash write failed.");
        return err;
    }

    return err;
}

/**
 * @brief       Erase a sector
 * @param       offset    : erase start address
 * @retval      ESP_OK: success; other: failure
 */
esp_err_t images_partition_erase_sector(uint32_t offset)
{
    esp_err_t err;

    err = esp_partition_erase_range(storage_partition, offset, SECTOR_SIZE);
    
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Flash erase failed.");
        return err;
    }

    return err;
}

/**
 * @brief       Show the current image update progress
 * @param       x, y    : coordinates
 * @param       size    : image size
 * @param       totsize : total file size
 * @param       pos     : current file pointer position
 * @param       color   : image color
 * @retval      none
 */
static void images_progress_show(uint16_t x, uint16_t y, uint8_t size, uint32_t totsize, uint32_t pos, uint16_t color)
{
    float prog;
    uint8_t t = 0XFF;
    prog = (float)pos / totsize;
    prog *= 100;

    if (t != prog)
    {
        lcd_show_string(x + 3 * size / 2, y, 240, 320, size, "%", color);
        t = prog;

        if (t > 100) t = 100;

        lcd_show_num(x, y, t, 3, size, color);  /* Display the value */
    }
}

/**
 * @brief       Update one image library
 * @param       x, y    : display position of the prompt
 * @param       size    : prompt image size
 * @param       fpath   : image path
 * @param       fx      : content to update
 *   @arg                 0, atk01;
 *   @Arg                 1, atk02;
 *   @arg                 2, atk03;
 *   @arg                 3, atk04;
 *   @arg                 4, atk05;
 * @param       color   : image color
 * @retval      0, success; other, error code;
 */
static uint8_t images_update_imagex(uint16_t x, uint16_t y, uint8_t size, uint8_t *fpath, uint8_t fx, uint16_t color)
{
    uint32_t flashaddr = 0;
    FIL *fftemp;
    uint8_t *tempbuf;
    uint8_t res;
    uint16_t bread;
    uint32_t offx = 0;
    uint8_t rval = 0;

    fftemp = (FIL *)malloc(sizeof(FIL));  /* Allocate memory */

    if (fftemp == NULL)rval = 1;

    tempbuf = malloc(4096);               /* Allocate 4096 bytes */

    if (tempbuf == NULL)rval = 1;

    res = f_open(fftemp, (const TCHAR *)fpath, FA_READ);

    if (res) rval = 2;   /* Failed to open the file */

    if (rval == 0)
    {
        switch (fx)
        {
            case 0:
                g_ftinfo.lvgl_camera_addr = IMAGEINFOADDR + sizeof(g_ftinfo);
                g_ftinfo.lvgl_camera_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_camera_addr;
                break;
            case 1:
                g_ftinfo.lvgl_file_addr = g_ftinfo.lvgl_camera_addr + g_ftinfo.lvgl_camera_size;
                g_ftinfo.lvgl_file_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_file_addr;
                break;
            case 2:
                g_ftinfo.lvgl_video_addr = g_ftinfo.lvgl_file_addr + g_ftinfo.lvgl_file_size;
                g_ftinfo.lvgl_video_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_video_addr;
                break;
            case 3:
                g_ftinfo.lvgl_setting_addr = g_ftinfo.lvgl_video_addr + g_ftinfo.lvgl_video_size;
                g_ftinfo.lvgl_setting_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_setting_addr;
                break;
            case 4:
                g_ftinfo.lvgl_weather_addr = g_ftinfo.lvgl_setting_addr + g_ftinfo.lvgl_setting_size;
                g_ftinfo.lvgl_weather_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_weather_addr;
                break;
            case 5:
                g_ftinfo.lvgl_measure_addr = g_ftinfo.lvgl_weather_addr + g_ftinfo.lvgl_weather_size;
                g_ftinfo.lvgl_measure_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_measure_addr;
                break;
            case 6:
                g_ftinfo.lvgl_photo_addr = g_ftinfo.lvgl_measure_addr + g_ftinfo.lvgl_measure_size;
                g_ftinfo.lvgl_photo_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_photo_addr;
                break;
            case 7:
                g_ftinfo.lvgl_music_addr = g_ftinfo.lvgl_photo_addr + g_ftinfo.lvgl_photo_size;
                g_ftinfo.lvgl_music_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_music_addr;
                break;
            case 8:
                g_ftinfo.lvgl_calendar_addr = g_ftinfo.lvgl_music_addr + g_ftinfo.lvgl_music_size;
                g_ftinfo.lvgl_calendar_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_calendar_addr;
                break;
            case 9:
                g_ftinfo.lvgl_background_addr = g_ftinfo.lvgl_calendar_addr + g_ftinfo.lvgl_calendar_size;
                g_ftinfo.lvgl_background_size = fftemp->obj.objsize;
                flashaddr = g_ftinfo.lvgl_background_addr;
                break;
        }

        while (res == FR_OK)            /* Execute in a loop */
        {
            res = f_read(fftemp, tempbuf, 4096, (UINT *)&bread);                /* Read data */

            if (res != FR_OK) break;    /* Execution error */

            images_partition_write(tempbuf, offx + flashaddr, bread);          /* Write bread bytes starting from 0 */
            offx += bread;
            images_progress_show(x, y, size, fftemp->obj.objsize, offx, color); /* Show progress */

            if (bread != 4096) break;   /* Finished reading */
        }

        f_close(fftemp);
    }

    free(fftemp);     /* Free memory */
    free(tempbuf);    /* Free memory */
    return res;
}

/**
 * @brief       Update the image files
 *   @note      All image libraries are updated together (UNIGBK, GBK12, GBK16, GBK24, GBK32)
 * @param       x, y    : display position of the prompt
 * @param       size    : prompt image size
 * @param       src     : disk that is the source of the image library
 *   @arg                 "0:", SD card;
 *   @arg                 "1:", FLASH disk
 * @param       color   : image color
 * @retval      0, success; other, error code;
 */
uint8_t images_update_image(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color)
{
    uint8_t *pname;
    uint32_t *buf;
    uint8_t res = 0;
    uint16_t i, j;
    FIL *fftemp;
    uint8_t rval = 0;
    res = 0XFF;
    g_ftinfo.imageok = 0XFF;

    pname = malloc(100);                    /* Allocate 100 bytes */
    buf = malloc(4096);                     /* Allocate 4K bytes */
    fftemp = (FIL *)malloc(sizeof(FIL));    /* Allocate memory */

    if (buf == NULL || pname == NULL || fftemp == NULL)
    {
        free(fftemp);
        free(pname);
        free(buf);
        return 5;   /* Memory allocation failed */
    }

    for (i = 0; i < IMAGE_GBK_NUM; i++)     /* First check that the files atk01, atk02, atk03, money are valid */
    {
        strcpy((char *)pname, (char *)src);                  /* Copy src into pname */
        strcat((char *)pname, (char *)IMAGE_GBK_PATH[i]);    /* Append the specific file path */
        res = f_open(fftemp, (const TCHAR *)pname, FA_READ); /* Try to open */

        if (res)
        {
            rval |= 1 << 7;     /* Mark file open failure */
            break;              /* On error, exit immediately */
        }
    }

    free(fftemp);               /* Free memory */

    if (rval == 0)  /* All image library files exist */
    {
        lcd_show_string(x, y, 240, 320, size, "Erasing sectors... ", color);    /* Indicate that sectors are being erased */

        for (i = 0; i < IMAGESECSIZE; i++)          /* Erase the image library area first to improve write speed */
        {
            images_progress_show(x + 20 * size / 2, y, size, IMAGESECSIZE, i, color);           /* Show progress */
            images_partition_read((uint8_t *)buf, ((IMAGEINFOADDR / 4096) + i) * 4096, 4096);   /* Read the contents of the whole sector */

            for (j = 0; j < 1024; j++)              /* Verify the data */
            {
                if (buf[j] != 0XFFFFFFFF) break;    /* Erase needed */
            }

            if (j != 1024)
            {
                images_partition_erase_sector(((IMAGEINFOADDR / 4096) + i) * 4096);     /* Sector that needs erasing */
            }
        }

        for (i = 0; i < IMAGE_UPDATE_REMIND_NUM; i++) /* Update UNIGBK, GBK12, GBK16, GBK24 in turn */
        {
            lcd_show_string(x, y, 240, 320, size, IMAGE_UPDATE_REMIND_TBL[i], color);
            strcpy((char *)pname, (char *)src);                 /* Copy src into pname */
            strcat((char *)pname, (char *)IMAGE_GBK_PATH[i]);   /* Append the specific file path */
            res = images_update_imagex(x + 20 * size / 2, y, size, pname, i, color);    /* Update the library */

            if (res)
            {
                free(buf);
                free(pname);
                return 1 + i;
            }
        }

        /* All updates completed */
        g_ftinfo.imageok = 0xBB;
        images_partition_write((uint8_t *)&g_ftinfo, IMAGEINFOADDR, sizeof(g_ftinfo));    /* Save the library information */
    }

    free(pname);    /* Free memory */
    free(buf);      /* Free memory */

    return rval;  
}

/**
 * @brief       Initialize images
 * @param       none
 * @retval      0, image library intact; other, image library missing;
 */
uint8_t images_init(void)
{
    uint8_t t = 0;

    storage_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "storage");
    
    if (storage_partition == NULL)
    {
        ESP_LOGE(TAG, "Flash partition not found.");
        return 1;
    }

    while (t < 10)  /* Read 10 times in a row; if all fail, the image library must be updated */
    {
        t++;
        images_partition_read((uint8_t *)&g_ftinfo, IMAGEINFOADDR, sizeof(g_ftinfo)); /* Read 10 times in a row; if all fail, the image library must be updated */

        if (g_ftinfo.imageok == 0xBB)
        {
            break;
        }
        
        vTaskDelay(20);
    }

    if (g_ftinfo.imageok != 0xBB)
    {
        return 1;
    }
    
    return 0;
}

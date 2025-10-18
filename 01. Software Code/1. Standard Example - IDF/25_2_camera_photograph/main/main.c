/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file main.c
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief Camera Experiment
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "camera.h"
#include "led.h"
#include "lcd.h"
#include "spi_sdcard.h"
#include "jpegd2.h"
#include "sdmmc_cmd.h"
#include "exfuns.h"

/* TASK3 Task Configuration
 * include: Task handle TaskPriority Stack size createTask
 */
#define TASK3_PRIO      4                   /* TaskPriority */
#define TASK3_STK_SIZE  5*1024              /* Task stack size */
TaskHandle_t            Task3Task_Handler;  /* Task handle */
void task3(void *pvParameters);             /* Task functions */
SemaphoreHandle_t BinarySemaphore;          /* Binary semaphore handle */
i2c_obj_t i2c0_master;
uint8_t *rgb565 = NULL;
static const char *TAG = "main";
uint8_t sd_check_en = 0;                    /* sdCardDetection mark */
extern sdmmc_card_t *card;                  /* SD/MMC card structure */

/**
 * @brief       getpathpathDown,Total number of target files
 * @param       path : path
 * @retval      Total number of valid files
 */
uint16_t pic_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;                                    /* Temporary Directory */
    FILINFO *tfileinfo;                             /* Temporary file information */
    tfileinfo = (FILINFO *)malloc(sizeof(FILINFO)); /* Apply for memory */
    res = f_opendir(&tdir, (const TCHAR *)path);    /* Open the directory */

    if (res == FR_OK && tfileinfo)
    {
        while (1)                                   /* Query the total number of valid files */
        {
            res = f_readdir(&tdir, tfileinfo);      /* Read a file in the directory */

            if (res != FR_OK || tfileinfo->fname[0] == 0)break; /* An error/It's the end,quit */
            res = exfuns_file_type(tfileinfo->fname);

            if ((res & 0X0F) != 0X00)               /* Take the lower four digits,See if it is a picture file */
            {
                rval++;                             /* Increase the number of valid files1 */
            }
        }
    }

    free(tfileinfo);                                /* Free memory */
    return rval;
}

/**
 * @brief program entry
 * @param None
 * @retval None
 */
void app_main(void)
{
    unsigned long i = 0;
    unsigned long j = 0;
    uint8_t key = 0;

    esp_err_t ret;
    
    ret = nvs_flash_init();             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                         /* initializationLED */
    i2c0_master = iic_init(I2C_NUM_0);  /* initializationIIC0 */
    spi2_init();                        /* Initialize SPI2 */
    xl9555_init(i2c0_master);           /* Initialize XL9555 */
    lcd_init();                         /* Initialize LCD */

    while (sd_spi_init())               /* Not detectedSDCard */
    {
        lcd_show_string(30, 50, 200, 16, 16, "SD Card Failed!", RED);
        vTaskDelay(200);
        lcd_fill(30, 50, 200 + 30, 50 + 16, WHITE);
        vTaskDelay(200);
        sd_check_en = 0;
    }

    sd_check_en = 1;

    lcd_show_string(30, 50, 200, 16, 16, "ESP32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "CAMERA TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    /* initializationCamera */
    while (camera_init())
    {
        lcd_show_string(30, 110, 200, 16, 16, "CAMERA Fail!", BLUE);
        vTaskDelay(500);
    }

    rgb565 = malloc(240 * 320 * 2);

    if (NULL == rgb565)
    {
        ESP_LOGE(TAG, "can't alloc memory for rgb565 buffer");
    }

    lcd_clear(BLACK);

    BinarySemaphore = xSemaphoreCreateBinary();
    /* Create Task 3 */
    xTaskCreatePinnedToCore((TaskFunction_t )task3,                 /* Task functions */
                            (const char*    )"task3",               /* Task name */
                            (uint16_t       )TASK3_STK_SIZE,        /* Task stack size */
                            (void*          )NULL,                  /* Passed in toTask functionsParameters of */
                            (UBaseType_t    )TASK3_PRIO,            /* TaskPriority */
                            (TaskHandle_t*  )&Task3Task_Handler,    /* Task handle */
                            (BaseType_t     ) 0);                   /* Which kernel does this task run */

    while (1)
    {
        key = xl9555_key_scan(0);
        camera_fb_t *pic = esp_camera_fb_get();

        if (pic)
        {
            mjpegdraw(pic->buf, pic->len, (uint8_t *)rgb565, NULL);

            lcd_set_window(0, 0, 0 + pic->width - 1, 0 + pic->height - 1);

            if (key == KEY0_PRES)
            {
                /* lcd_bufStorage camera one frameRGBdata */
                for (j = 0; j < pic->width * pic->height; j++)
                {
                    lcd_buf[2 * j] = (pic->buf[2 * i]) ;
                    lcd_buf[2 * j + 1] =  (pic->buf[2 * i + 1]);
                    i ++;
                }

                xSemaphoreGive(BinarySemaphore);                    /* Release binary semaphore */
            }

            /* deal withSDCardRelease the mount */
            if (sd_check_en == 1)
            {
                if (sdmmc_get_status(card) != ESP_OK)
                {
                    sd_check_en = 0;
                }
            }
            else
            {
                if (sd_spi_init() == ESP_OK)
                {
                    if (sdmmc_get_status(card) == ESP_OK)
                    {
                        sd_check_en = 1;
                    }
                }
            }
            
            /* For example：96*96*2/1536 = 12;point12SendRGBdata */
            for(j = 0; j < (pic->width * pic->height * 2 / LCD_BUF_SIZE); j++)
            {
                /* &lcd_buf[j * LCD_BUF_SIZE] Offset address senddata */
                lcd_write_data(&rgb565[j * LCD_BUF_SIZE] , LCD_BUF_SIZE);
            }

            esp_camera_fb_return(pic);
        }
        else
        {
            ESP_LOGE(TAG, "Get frame failed");
        }

        i = 0;
        pic = NULL;
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    free(rgb565);
}

/**
 * @brief       task3
 * @param       pvParameters : Pass in parameters(Not used)
 * @retval      none
 */
void task3(void *pvParameters)
{
    pvParameters = pvParameters;
    char file_name[30];
    uint32_t pictureNumber = 0;
    uint8_t res = 0;
    size_t writelen = 0;
    FIL *fftemp;
    res = exfuns_init();                                            /* forfatfsRelated variables request memory */
    pictureNumber = pic_get_tnum("0:/PICTURE");                     /* Get the total number of valid files */
    pictureNumber = pictureNumber + 1;

    while (1)
    {
        xSemaphoreTake(BinarySemaphore, portMAX_DELAY);             /* Get binary semaphore */
        
        /* SDCardMounted，Only take photos */
        if (sd_check_en == 1)
        {
            sprintf(file_name, "0:/PICTURE/img%ld.jpg", pictureNumber);
            fftemp = (FIL *)malloc(sizeof(FIL));                    /* pointMemory */
            res = f_open(fftemp, (const TCHAR *)file_name, FA_WRITE | FA_CREATE_NEW);   /* Try to open */

            if (res != FR_OK)
            {
                ESP_LOGE(TAG, "img open err\r\n");
            }

            f_write(fftemp, (const void *)lcd_buf, sizeof(lcd_buf), &writelen); /* Write headerdata */

            if (writelen != sizeof(lcd_buf))
            {
                ESP_LOGE(TAG, "img Write err");
            }
            else
            {
                ESP_LOGI(TAG, "write buff len %d byte", writelen);
                pictureNumber++;
            }

            f_close(fftemp);
            free(fftemp);
        }
    }
}
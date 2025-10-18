/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Image display experiment
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "led.h"
#include "spi.h"
#include "iic.h"
#include "lcd.h"
#include "xl9555.h"
#include "spi_sdcard.h"
#include "math.h"
#include "exfuns.h"
#include "piclib.h"
#include "ff.h"
#include "fonts.h"
#include "text.h"

i2c_obj_t i2c0_master;

/**
 * @brief       getpathpathDown,Total number of target files
 * @param       path : path
 * @retval      Total number of valid files
 */
uint16_t pic_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;                                    /* Temporary Table of contents */
    FILINFO *tfileinfo;                             /* temporaryFile information */
    tfileinfo = (FILINFO *)malloc(sizeof(FILINFO)); /* Apply for memory */
    res = f_opendir(&tdir, (const TCHAR *)path);    /* Open the directory */

    if (res == FR_OK && tfileinfo)
    {
        while (1)                                   /* Query the total number of valid files */
        {
            res = f_readdir(&tdir, tfileinfo);      /* Read a file in the directory */

            if (res != FR_OK || tfileinfo->fname[0] == 0)break; /* An error/It's the end,quit */
            res = exfuns_file_type(tfileinfo->fname);

            if ((res & 0X0F) != 0X00)               /* Take the lower four digits,See if it isPicture files */
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
    esp_err_t ret = 0;
    uint8_t res = 0;
    FF_DIR picdir;                                      /* Picture catalog */
    FILINFO *picfileinfo;                               /* File information */
    char *pname;                                        /* File name with path */
    uint16_t totpicnum;                                 /* Total number of picture files */
    uint16_t curindex = 0;                              /* Image Current Index */
    uint8_t key = 0;                                    /* Key value */
    uint8_t pause = 0;                                  /* Pause mark */
    uint8_t t;
    uint16_t temp;
    uint32_t *picoffsettbl;                             /* Picture filesoffsetIndex table */

    ret = nvs_flash_init();                             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                                         /* initializationLED */
    i2c0_master = iic_init(I2C_NUM_0);                  /* initializationIIC0 */
    spi2_init();                                        /* initializationSPI */
    xl9555_init(i2c0_master);                           /* Initialize the IO expansion chip */  
    lcd_init();                                         /* Initialize LCD */

    while (sd_spi_init())                               /* Not detectedSDCard */
    {
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Error!", RED);
        vTaskDelay(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        vTaskDelay(500);
    }

    res = exfuns_init();                                /* forfatfsRelated variables request memory */

    while (fonts_init())                                /* Check the font library */
    {
        lcd_clear(WHITE);                               /* Clear the screen */
        lcd_show_string(30, 30, 200, 16, 16, "ESP32-S3", RED);
        
        key = fonts_update_font(30, 50, 16, (uint8_t *)"0:", RED);  /* Update the font library */

        while (key)                                     /* Update failed */
        {
            lcd_show_string(30, 50, 200, 16, 16, "Font Update Failed!", RED);
            vTaskDelay(200);
            lcd_fill(20, 50, 200 + 20, 90 + 16, WHITE);
            vTaskDelay(200);
        }

        lcd_show_string(30, 50, 200, 16, 16, "Font Update Success!   ", RED);
        vTaskDelay(1500);
        lcd_clear(WHITE);                               /* Clear the screen */
    }

    text_show_string(30, 50, 200, 16, "STM32Development Board",16,0, RED);
    text_show_string(30, 70, 200, 16, "Image display experiment", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "@ALIENTEK", 16, 0, RED);
    text_show_string(30, 110, 200, 16, "KEY0:NEXT KEY1:PREV", 16, 0, RED);
    text_show_string(30, 130, 200, 16, "KEY_UP:PAUSE:", 16, 0, RED);

    while (f_opendir(&picdir, "0:/PICTURE"))            /* Open the picture folder */
    {
        text_show_string(30, 150, 240, 16, "PICTUREFolder Error!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(30, 150, 240, 186, WHITE);             /* Clear the display */
        vTaskDelay(200);
    }

    totpicnum = pic_get_tnum("0:/PICTURE");             /* Get the total number of valid files */
 
    while (totpicnum == NULL)                           /* Picture filesfor0 */
    {
        text_show_string(30, 150, 240, 16, "No picture files!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(30, 150, 240, 186, WHITE);             /* Clear the display */
        vTaskDelay(200);
    }

    picfileinfo = (FILINFO *)malloc(sizeof(FILINFO));   /* Apply for memory */
    pname = malloc(255 * 2 + 1);                        /* forFile name with pathallocate memory */
    picoffsettbl = malloc(4 * totpicnum);               /* Apply for 4*totpicnum bytes of memory to store image index */

    while (!picfileinfo || !pname || !picoffsettbl)     /* Memory allocation error */
    {
        text_show_string(30, 150, 240, 16, "Memory allocation failed!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(30, 150, 240, 186, WHITE);             /* Clear the display */
        vTaskDelay(200);
    }

    /* Record index */
    res = f_opendir(&picdir, "0:/PICTURE");             /* Open the directory */

    if (res == FR_OK)
    {
        curindex = 0;                                   /* The current index is 0 */

        while (1)                                       /* Query all */
        {
            temp = picdir.dptr;                         /* Record the current dptr offset */
            res = f_readdir(&picdir, picfileinfo);      /* Read a file in the directory */
            if (res != FR_OK || picfileinfo->fname[0] == 0)break;   /* An error/It's the end,quit */

            res = exfuns_file_type(picfileinfo->fname);

            if ((res & 0X0F) != 0X00)                   /* Take the top four digits to see if it is a picture file */
            {
                picoffsettbl[curindex] = temp;          /* Record index */
                curindex++;
            }
        }
    }

    text_show_string(30, 150, 240, 16, "Start displaying...", 16, 0, RED);
    vTaskDelay(1500);
    piclib_init();                                                              /* initializationDraw pictures */
    curindex = 0;                                                               /* from0Start displaying */
    res = f_opendir(&picdir, (const TCHAR *)"0:/PICTURE");                      /* Open the directory */

    while (res == FR_OK)                                                        /* Open successfully */
    {
        dir_sdi(&picdir, picoffsettbl[curindex]);                               /* Change the current directory index */
        res = f_readdir(&picdir, picfileinfo);                                  /* Read a file in the directory */

        if (res != FR_OK || picfileinfo->fname[0] == 0)break;                   /* An error/It's the end,quit */

        strcpy((char *)pname, "0:/PICTURE/");                                   /* Copy path (directory) */
        strcat((char *)pname, (const char *)picfileinfo->fname);                /* Connect the file name behind */
        lcd_clear(BLACK);
        piclib_ai_load_picfile(pname, 0, 0, lcd_self.width, lcd_self.height);   /* Show picture */
        text_show_string(2, 2, lcd_self.width, 16, (char *)pname, 16, 0, RED);  /* Show image name */
        t = 0;

        while (1)
        {
            key = xl9555_key_scan(0);   /* Scan button */

            if (t > 250)key = 1;        /* Simulate and press KEY0 once */

            if ((t % 20) == 0)
            {
                LED_TOGGLE();           /* LEDFlashing,Prompt program is running. */
            }

            if (key == KEY1_PRES)       /* Previous */
            {
                if (curindex)
                {
                    curindex--;
                }
                else
                {
                    curindex = totpicnum - 1;
                }
                
                break;
            }
            else if (key == KEY0_PRES)      /* Next */
            {
                curindex++;

                if (curindex >= totpicnum)
                {
                    curindex = 0;           /* By the end,automaticfromStart */
                }

                break;
            }
            else if (key == KEY3_PRES)
            {
                pause = !pause;
                LED(pause);                 /* LED1 lights up when paused. */
            }

            if (pause == 0)t++;

            vTaskDelay(10);
        }

        res = 0;
    }

    free(picfileinfo);    /* Free memory */
    free(pname);          /* Free memory */
    free(picoffsettbl);   /* Free memory */
}
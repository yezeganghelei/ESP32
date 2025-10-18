/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Chinese character display experiment
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "led.h"
#include "iic.h"
#include "spi.h"
#include "lcd.h"
#include "xl9555.h"
#include "nvs_flash.h"
#include "fonts.h"
#include "spi_sdcard.h"
#include "text.h"

i2c_obj_t i2c0_master;

/**
 * @brief       Program entry
 * @param       none
 * @retval      none
 */
void app_main(void)
{
    esp_err_t ret;
    uint8_t t;
    uint8_t key;
    uint8_t res;
    uint32_t fontcnt;
    uint8_t i;
    uint8_t j;
    uint8_t fontx[2];

    ret = nvs_flash_init();                             /* Initialize NVS */
    
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    led_init();                                         /* initializationLED */
    i2c0_master = iic_init(I2C_NUM_0);                  /* initializationIIC0 */
    spi2_init();                                        /* initializationSPI */
    xl9555_init(i2c0_master);                           /* Initialize the IO expansion chip */  
    lcd_init();                                         /* Initialize LCD */

    while (sd_spi_init())                               /* Not detectedSDCard */
    {
        lcd_show_string(30, 50, 200, 16, 16, "SD Card Failed!", RED);
        vTaskDelay(200);
        lcd_fill(30, 50, 200 + 30, 50 + 16, WHITE);
        vTaskDelay(200);
    }

    while (fonts_init())                                /* Check the font library */
    {
UPD:
        lcd_clear(WHITE);                               /* clear screen */
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
        lcd_clear(WHITE);                               /* clear screen */
    }
   
    text_show_string(30, 30, 200, 16, "ESP32-S3Development Board", 16, 0, RED);
    text_show_string(30, 50, 200, 16, "GBKFont library testing program", 16, 0, RED);
    text_show_string(30, 70, 200, 16, "ATOM@ALIENTEK", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "KEY0: Update font library", 16, 0, RED);
    
    text_show_string(30, 110, 200, 16, "High bytes in the inner code:", 16, 0, BLUE);
    text_show_string(30, 130, 200, 16, "Low bytes in the inner code:", 16, 0, BLUE);
    text_show_string(30, 150, 200, 16, "Chinese character counter:", 16, 0, BLUE);
    
    text_show_string(30, 180, 200, 24, "The corresponding Chinese characters are:", 24, 0, BLUE);
    text_show_string(30, 204, 200, 16, "The corresponding Chinese characters are:", 16, 0, BLUE);
    text_show_string(30, 220, 200, 12, "The corresponding Chinese characters are:", 12, 0, BLUE);

    while (1)
    {
        fontcnt = 0;
        
        for (i = 0x81; i < 0xFF; i++)                                           /* GBKHigh bytes in the inner codeThe range is0x81~0xFE */
        {
            fontx[0] = i;
            lcd_show_num(118, 110, i, 3, 16, BLUE);                             /* Display the inner code high bytes */
            
            for (j = 0x40; j < 0xFE; j ++)                                      /* The low byte range of GBK internal code is 0x40~0x7E, 0x80~0xFE) */
            {
                if (j == 0x7F)
                {
                    continue;
                }
                
                fontcnt++;
                lcd_show_num(118, 130, j, 3, 16, BLUE);                         /* Display the inner code low bytes */
                lcd_show_num(118, 150, fontcnt, 5, 16, BLUE);                   /* Chinese character count display */
                fontx[1] = j;
                text_show_font(30 + 132, 180, fontx, 24, 0, BLUE);
                text_show_font(30 + 144, 204, fontx, 16, 0, BLUE);
                text_show_font(30 + 108, 220, fontx, 12, 0, BLUE);
                
                t = 200;
                
                while ((t --) != 0)                                              /* Delay，Scan the keys simultaneously */
                {
                    vTaskDelay(1);
                    
                    key = xl9555_key_scan(0);
                    
                    if (key == KEY0_PRES)
                    {
                        goto UPD;                                               /* Jump toUPDLocation（Forced update of font library） */
                    }
                }
                
                LED_TOGGLE();
            }
        }
    }
}
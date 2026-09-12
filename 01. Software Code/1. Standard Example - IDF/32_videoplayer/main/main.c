/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Picture display experiment
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
#include "videoplay.h"
#include "es8388.h"
#include "i2s.h"
#include "fonts.h"
#include "text.h"

i2c_obj_t i2c0_master;

/**
 * @brief program entry
 * @param None
 * @retval None
 */
void app_main(void)
{
    esp_err_t ret = 0;
    uint8_t key = 0;

    ret = nvs_flash_init();                             /* initializationNVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                                         /* initializationLED */
    i2c0_master = iic_init(I2C_NUM_0);                  /* initializationIIC0 */
    spi2_init();                                        /* Initialize SPI */
    xl9555_init(i2c0_master);                           /* initializationIOExpand chip */  
    lcd_init();                                         /* Initialize LCD */

    es8388_init(i2c0_master);                           /* ES8388initialization */
    es8388_adda_cfg(1, 0);                              /* turn onDACclosureADC */
    es8388_input_cfg(0);
    es8388_output_cfg(1, 1);                            /* DACSelect channel output */
    es8388_hpvol_set(33);                               /* Set the headphone volume */
    es8388_spkvol_set(33);                              /* Set the speaker volume */
    i2s_init();                                         /* I2Sinitialization */
    vTaskDelay(1000);
    xl9555_pin_write(SPK_EN_IO,0);                      /* Turn on the speaker */

    while (sd_spi_init())                               /* The SD card cannot be detected */
    {
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Error!", RED);
        vTaskDelay(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        vTaskDelay(500);
    }

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

    if (xl9555_key_scan(0) == KEY0_PRES)
    {
        fonts_update_font(30, 50, 16, (uint8_t *)"0:", RED);  /* Update the font library */
    }

    ret = exfuns_init();        /* Apply for memory for fatfs-related variables */

    text_show_string(30, 30, 200, 16, "ESP32Development board", 16, 0, RED);
    text_show_string(30, 50, 200, 16, "Video player experiment", 16, 0, RED);
    text_show_string(30, 70, 200, 16, "@ALIENTEK", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "KEY0:NEXT KEY2:PREV ", 16, 0, RED);
    text_show_string(30, 110, 200, 16, "KEY_UP:FF   KEY1:REW", 16, 0, RED);
    vTaskDelay(500);            /* Delay for experiment information display */

    while (1)
    {
        video_play();
    }
}
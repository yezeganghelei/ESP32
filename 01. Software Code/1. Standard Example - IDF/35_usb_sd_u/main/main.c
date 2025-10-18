/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       SDCard模拟Uplate experiment
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
#include "tud_sd.h"
#include "spi_sdcard.h"

i2c_obj_t i2c0_master;

/**
 * @brief program entry
 * @param none
 * @retval None
 */
void app_main(void)
{
    esp_err_t ret;

    ret = nvs_flash_init();                             /* initializationNVS */
    
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    led_init();                                         /* initializationLED */
    i2c0_master = iic_init(I2C_NUM_0);                  /* initializationIIC0 */
    spi2_init();                                        /* Initialize SPI */
    xl9555_init(i2c0_master);                           /* initializationIOexpansion chip */  
    lcd_init();                                         /* Initialize LCD */
    /* 显示experiment信息 */
    lcd_show_string(30, 50, 200, 16, 16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, 16, "USB SD TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "status:", RED);
    
    while (sd_spi_init())                               /* The SD card cannot be detected */
    {
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Error!", RED);
        vTaskDelay(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        vTaskDelay(500);
    }

    tud_usb_sd();                                       /* USBinitialization */

    while(1)
    {
        if ((g_usbdev.status & 0x0f) == 0x01)
        {
            lcd_show_string(110, 110, lcd_self.width, 16, 16, "connect success.....", BLUE);
        }
        else if ((g_usbdev.status & 0x0f) == 0x00)
        {
            lcd_show_string(110, 110, lcd_self.width, 16, 16, "connect fail........", BLUE);
        }

        LED_TOGGLE();
        vTaskDelay(500);
    }
}
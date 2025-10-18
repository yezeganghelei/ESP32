/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       USBVirtual serial port（Slave）experiment
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "led.h"
#include "spi.h"
#include "iic.h"
#include "lcd.h"
#include "xl9555.h"
#include "tud_usart.h"

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
    spi2_init();                                        /* initializationSPI */
    xl9555_init(i2c0_master);                           /* initializationIOExpanding chip */  
    lcd_init();                                         /* Initialize LCD */
    
    /* 显示experiment信息 */
    lcd_show_string(30, 50, 200, 16, 16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, 16, "USB USART TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    tud_usb_usart();                                    /* USBinitialization */
    
    while(1)
    {
        LED_TOGGLE();
        vTaskDelay(500);
    }
}
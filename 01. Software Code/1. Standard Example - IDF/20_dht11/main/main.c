/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       DHT11Digital temperature and humidity sensor experiment
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "led.h"
#include "iic.h"
#include "spi.h"
#include "lcd.h"
#include "xl9555.h"
#include "dht11.h"

i2c_obj_t i2c0_master;

/**
 * @brief       Program entry
 * @param       none
 * @retval      none
 */
void app_main(void)
{
    uint8_t err;
    uint8_t t = 0;
    uint8_t temperature;
    uint8_t humidity;
    esp_err_t ret;
    
    ret = nvs_flash_init();                                         /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                                                     /* initializationLED */
    i2c0_master = iic_init(I2C_NUM_0);                              /* initializationIIC0 */
    spi2_init();                                                    /* Initialize SPI2 */
    xl9555_init(i2c0_master);                                       /* Initialize XL9555 */
    lcd_init();                                                     /* Initialize LCD */

    lcd_show_string(30, 50, 200, 16, 16, "ESP32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DHT11 TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    /* Initialize DHT11 digital temperature and humidity sensor */
    err = dht11_init();
    
    if (err != 0)
    {
        while (1)
        {
            lcd_show_string(30, 110, 200, 16, 16, "DHT11 Error", RED);
            vTaskDelay(200);
            lcd_fill(30, 110, 239, 130 + 16, WHITE);
            vTaskDelay(200);
        }
    }

    lcd_show_string(30, 110, 200, 16, 16, "DHT11 OK", RED);
    lcd_show_string(30, 130, 200, 16, 16, "Temp:  C", BLUE);
    lcd_show_string(30, 150, 200, 16, 16, "Humi:  %", BLUE);
    
    while (1)
    {
        if (t % 10 == 0)                                            /* Every100msRead once */
        {
            dht11_read_data(&temperature, &humidity);               /* Read the temperature and humidity value */
            lcd_show_num(30 + 40, 130, temperature, 2, 16, BLUE);   /* Display temperature */
            lcd_show_num(30 + 40, 150, humidity, 2, 16, BLUE);      /* Show humidity */
        }

        vTaskDelay(10);
        t++;

        if (t == 20)
        {
            t = 0;
            LED_TOGGLE();                                           /* LED flashing */
        }
    }
}
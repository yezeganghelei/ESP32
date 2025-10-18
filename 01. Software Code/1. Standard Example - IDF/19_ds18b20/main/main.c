/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       DS18B20Digital temperature sensorexperiment
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
#include "iic.h"
#include "spi.h"
#include "xl9555.h"
#include "lcd.h"
#include "ds18b20.h"

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
    short temperature;
    esp_err_t ret;
    
    ret = nvs_flash_init();                                                 /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                                                             /* initializationLED */
    i2c0_master = iic_init(I2C_NUM_0);                                      /* initializationIIC0 */
    spi2_init();                                                            /* initializationSPI */
    xl9555_init(i2c0_master);                                               /* Initialize XL9555 */
    lcd_init();                                                             /* Initialize LCD */

    lcd_show_string(30, 50, 200, 16, 16, "ESP32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DS18B20 TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    /* initializationDS18B20Digital temperature sensor */
    err = ds18b20_init();

    if (err != 0)
    {
        while (1)
        {
            lcd_show_string(30, 110, 200, 16, 16, "DS18B20 Error", RED);
            vTaskDelay(200);
            lcd_fill(30, 110, 239, 130 + 16, WHITE);
            vTaskDelay(200);
        }
    }
    lcd_show_string(30, 110, 200, 16, 16, "DS18B20 OK", RED);
    lcd_show_string(30, 130, 200, 16, 16, "Temp:   . °C", BLUE);
    
    while (1)
    {
        if (t % 10 == 0)                                                    /* Every100msRead once */
        {
            temperature = ds18b20_get_temperature();

            if (temperature < 0)
            {
                lcd_show_char(30 + 40, 130, '-', 16, 0, BLUE);              /* Show negative sign */
                temperature = -temperature;                                 /* Turn to positive number */
            }
            else
            {
                lcd_show_char(30 + 40, 130, ' ', 16, 0, BLUE);              /* Remove the negative sign */
            }
            lcd_show_num(30 + 40 + 8, 130, temperature / 10, 2, 16, BLUE);  /* Show positive part */
            lcd_show_num(30 + 40 + 32, 130, temperature % 10, 1, 16, BLUE); /* Show decimal parts */
        }

        vTaskDelay(10);
        t++;

        if (t == 20)
        {
            t = 0;
            LED_TOGGLE();                                                   /* LED flashing */
        }
    }
}
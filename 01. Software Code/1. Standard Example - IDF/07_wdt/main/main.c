/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       Watchdog experiment
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "led.h"
#include "key.h"
#include "wdt.h"

/**
 * @brief       Program entry
 * @param       none
 * @retval      none
 */
void app_main(void)
{
    esp_err_t rets;
    
    rets = nvs_flash_init();            /* Initialize NVS */

    if (rets == ESP_ERR_NVS_NO_FREE_PAGES || rets == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        rets = nvs_flash_init();
    }

    led_init();                         /* Initialize LED */
    key_init();                         /* Initialize button */
    wdt_init(5000, 1000000);            /* Initialize the timer */
    LED(0);                             /* LED light always on */

    while (1)
    {
        if (key_scan(0) == BOOT_PRES)   /* ifBOOTPress toFeed the dog */
        {
            restart_timer(1000000);     /* Feed the dog */
        }

        vTaskDelay(10);                 /* LED flashing */
    }
}
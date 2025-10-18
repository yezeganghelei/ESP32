/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       high resolutionTimer(ESPTimer)experiment
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
#include "esptim.h"

/**
 * @brief       Program entry
 * @param       none
 * @retval      none
 */
void app_main(void)
{
    esp_err_t ret;
    
    ret = nvs_flash_init();         /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    led_init();                     /* Initialize LED */
    esptim_int_init(1000000);       /* Initialize the high-resolution timer, set the timer period to 1 second here.
                                       However, the function matters are calculated in units of subtle units.
                                       Therefore, 1 second is converted to 1000000 microseconds */
}
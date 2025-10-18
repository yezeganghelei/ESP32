/**
 ******************************************************************************
 * @file     main.c
 * @author   wocream team
 * @version  V1.0
 * @date     2023-08-26
 * @brief    LED
 ******************************************************************************
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "led.h"
#include "exit.h"

/**
 * @brief       main interface
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret;
    
    ret = nvs_flash_init(); 

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    led_init();             
    exit_init();            
    
    while(1) 
    {
        vTaskDelay(10);
    }
}
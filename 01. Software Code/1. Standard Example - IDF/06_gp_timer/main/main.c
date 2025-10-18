/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       General timer experiment
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "led.h"
#include "gptim.h"

/**
 * @brief       Program entry
 * @param       none
 * @retval      none
 */
void app_main(void)
{
    uint8_t record;
    esp_err_t ret;
    gptimer_event_t g_tim_evente;
    
    ret = nvs_flash_init();                                                                   /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    led_init();                                                                               /* Initialize LED */
    gptim_int_init(100, 1000000);                                                             /* Initialize the general timer */
    
    while (1)
    {
        record = 4;

        if (xQueueReceive(queue, &g_tim_evente, 2000))
        {
            ESP_LOGI("GPTIMER_ALARM", "Timer alarm, Count value： %llu", g_tim_evente.event_count);   /* Print the value obtained after a counting event occurs by the general timer */
            record--;
        }
        else
        {
            ESP_LOGW("GPTIMER_ALARM", "Missed a count event");
        }
    }
    vQueueDelete(queue);
}
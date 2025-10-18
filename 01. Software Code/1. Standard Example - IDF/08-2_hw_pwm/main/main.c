/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       Hardware changesPWMDuty Cycle Experiment
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "nvs_flash.h"
#include "pwm.h"

/**
 * @brief       Program entry
 * @param       none
 * @retval      none
 */
void app_main(void)
{
    esp_err_t ret;

    ret = nvs_flash_init();             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    pwm_init(13, 5000);                 /* Initialize PWM */

    while(1) 
    {
        vTaskDelay(10);
        pwm_set_duty(LEDC_PWM_DUTY);    /* Set duty cycle */
    }
}
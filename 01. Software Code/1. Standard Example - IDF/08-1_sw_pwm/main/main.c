/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       Software changesPWMDuty Cycle Experiment
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
    uint8_t dir = 1;
    uint16_t ledpwmval = 0;

    ret = nvs_flash_init(); /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    pwm_init(10, 1000);     /* initializationPWM */

    while(1) 
    {
        vTaskDelay(10);

        if (dir == 1)
        {
            ledpwmval += 5; /* dir==1 ledpwmvalIncrement */
        }
        else
        {
            ledpwmval -= 5; /* dir==0 ledpwmval decrement */
        }

        if (ledpwmval > 1005)
        {
            dir = 0;        /* ledpwmvalarrive1005back，Direction is decreasing */
        }

        if (ledpwmval < 5)
        {
            dir = 1;        /* ledpwmvalDecreased to5back，Change direction to increment */
        }

        /* Set duty cycle */
        pwm_set_duty(ledpwmval);
    }
}
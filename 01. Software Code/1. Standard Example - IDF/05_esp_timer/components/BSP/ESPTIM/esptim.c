/**
 ****************************************************************************************************
 * @file        esptim.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       High resolution timer（ESPTimer）Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "esptim.h"

/**
 * @brief       Initialization high precisionTimer（ESP_TIMER）
 * @param       tps: Timer cycle，In units of subtle（μs），Take one secondTimer cycleCome and execute onceTimerInterrupt，
 *                   Then heretps = 1s = 1000000μs
 * @retval      none
 */

void esptim_int_init(uint64_t tps)
{
    esp_timer_handle_t esp_tim_handle;                      /* Timer callback functionHandle */

    /* Define a timer structure */
    esp_timer_create_args_t tim_periodic_arg = {
        .callback = &esptim_callback,                       /* Set callback function */
        .arg = NULL,                                        /* No parameters are carried */
    };

    esp_timer_create(&tim_periodic_arg, &esp_tim_handle);   /* Create an event */
    esp_timer_start_periodic(esp_tim_handle, tps);          /* Triggered once per cycle */
}

/**
 * @brief       Timer callback function
 * @param       arg: No parameters are carried
 * @retval      none
 */
void esptim_callback(void *arg)
{
    LED_TOGGLE();
}
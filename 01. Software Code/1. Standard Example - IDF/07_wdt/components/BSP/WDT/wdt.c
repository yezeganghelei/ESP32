/**
 ****************************************************************************************************
 * @file        wdt.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       Task Watchdog Driver Code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "wdt.h"

esp_timer_handle_t esp_tim_handle;                          /* Timer callback function handle */

/**
 * @brief       Initialize task watchdog timer
 * @param       arr: Automatically reload value
 *              tps: Timer cycle
 */
void wdt_init(uint16_t arr, uint64_t tps)
{
    /* Define a timer structure */
    esp_timer_create_args_t tim_periodic_arg = {
    .callback =	&wdt_isr_handler,                           /* Set callback function */
    .arg = NULL,                                            /* No parameters are carried */
    };

    /* Create a timer event */
    esp_timer_create(&tim_periodic_arg, &esp_tim_handle);   /* Create an event */
    esp_timer_start_periodic(esp_tim_handle, tps);          /* Triggered once per cycle */
}

/**
 * @brief       Restart the currently running timer
 * @param       timeout: Timer timeout time，This timeout is based on subtlety as the basic unit of calculation，Therefore, the timeout time is set to1s，It needs to be converted into subtle（μs），Right nowtimeout = 1s = 1000000μs
 * @retval      none
 */
void restart_timer(uint64_t timeout)
{
    esp_timer_restart(esp_tim_handle, timeout);             /* Restart the currently running timer，Used to simulate the dog feeding process */
}

/**
 * @brief       Watchdog callback function
 * @param       arg: noneParameters passed in
 * @retval      none
 */
void IRAM_ATTR wdt_isr_handler(void *arg)
{
    esp_restart();                                          /* If the dog is not fed in time，Then the chip will be reset */
}
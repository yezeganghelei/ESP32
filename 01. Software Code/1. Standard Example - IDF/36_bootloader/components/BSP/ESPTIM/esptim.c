/**
 ******************************************************************************************************
 * @file esptim.c
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief High-resolution timer (ESP timer) driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************
 * @attention

 ******************************************************************************************************
 */

#include "esptim.h"

uint8_t frameup;
esp_timer_handle_t esp_tim_handle;                          /* Timer callback function handle */

/**
 * @brief       Initialize high-resolution timer
 * @param       arr: autoreload value
 * @param       tp: timer period
 * @retval      none
 */
void esptim_int_init(uint16_t arr, uint64_t tp)
{
    timer_config_t esp_timx_handle = {0};                   /* timer handle */

    /* Define a timer structure */
    esp_timer_create_args_t tim_periodic_arg = {
    .callback =	&TIM_PeriodElapsedCallback,                 /* Set callback function */
    .arg = NULL,                                            /* Does not carry parameters */
    };

    /* Configure the timer */
    esp_timx_handle.alarm_en = TIMER_ALARM_DIS;             /* Disable timer alarm */
    esp_timx_handle.counter_en = TIMER_START;               /* enable timer */
    esp_timx_handle.intr_type = TIMER_INTR_MAX;             /* Configure the timer interrupt mode */
    esp_timx_handle.counter_dir = TIMER_COUNT_UP;           /* Incremental Count Mode */
    esp_timx_handle.auto_reload = arr;                      /* Auto reload value */
    esp_timx_handle.clk_src = TIMER_SRC_CLK_DEFAULT;        /* Configure the timer clock source */

    esp_timer_create(&tim_periodic_arg, &esp_tim_handle);   /* Create the timer */
    esp_timer_start_periodic(esp_tim_handle, tp);           /* Trigger once per period */
}

/**
 * @brief       Timer callback function
 * @param       none
 * @retval      none
 */
void TIM_PeriodElapsedCallback(void *arg)
{
    frameup = 1;
}
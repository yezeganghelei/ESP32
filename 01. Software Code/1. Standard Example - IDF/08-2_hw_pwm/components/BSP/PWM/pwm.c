/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file led.c
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief PWM driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "pwm.h"

/**
 * @brief       initializationPWM
 * @param       resolution：PWM duty cycle resolution
 *              freq: PWM signal frequency
 * @retval      none
 */
void pwm_init(uint8_t resolution, uint16_t freq)
{
    ledc_timer_config_t ledc_timer;                 /* LEDCTimerHandle */
    ledc_channel_config_t ledc_channel;             /* LEDC channel configuration handle */

    /* Configure LEDC timer */
    ledc_timer.duty_resolution = resolution;        /* PWM duty cycle resolution */
    ledc_timer.freq_hz = freq;                      /* PWM signal frequency */
    ledc_timer.speed_mode = LEDC_PWM_MODE;          /* Timermodel */
    ledc_timer.timer_num = LEDC_PWM_TIMER;          /* Timer serial number */
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;             /* LEDC clock source */
    ledc_timer_config(&ledc_timer);                 /* Configure the timer */

    /* Configure LEDC channel */
    ledc_channel.gpio_num = LEDC_PWM_CH0_GPIO;      /* LED controller channel corresponding pins */
    ledc_channel.speed_mode = LEDC_PWM_MODE;        /* LEDC high-speed mode */
    ledc_channel.channel = LEDC_PWM_CH0_CHANNEL;    /* LEDCController channel number */
    ledc_channel.intr_type = LEDC_INTR_DISABLE;     /* LEDC Disabled Interrupt */
    ledc_channel.timer_sel = LEDC_PWM_TIMER;        /* Timer serial number */
    ledc_channel.duty = 0;                          /* Duty cycle value */
    ledc_channel_config(&ledc_channel);             /* Configure LEDC channel */

    ledc_fade_func_install(0);                      /* Enable gradient（This function is indispensable） */
}

/**
 * @brief PWM duty cycle settings
 * @param duty: PWM duty cycle
 * @retval None
 */
void pwm_set_duty(uint16_t duty)
{
    ledc_set_fade_with_time(LEDC_PWM_MODE, LEDC_PWM_CH0_CHANNEL, duty, LEDC_PWM_FADE_TIME);   /* set upDuty Cycleand the length of gradient */
    ledc_fade_start(LEDC_PWM_MODE, LEDC_PWM_CH0_CHANNEL, LEDC_FADE_NO_WAIT);                  /* Start to change */

    ledc_set_fade_with_time(LEDC_PWM_MODE, LEDC_PWM_CH0_CHANNEL, 0, LEDC_PWM_FADE_TIME);      /* set upDuty Cycleand the length of gradient */
    ledc_fade_start(LEDC_PWM_MODE, LEDC_PWM_CH0_CHANNEL, LEDC_FADE_NO_WAIT);                  /* Start to change */
}
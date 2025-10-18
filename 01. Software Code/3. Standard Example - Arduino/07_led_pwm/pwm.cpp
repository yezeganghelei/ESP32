/**
 ****************************************************************************************************
 * @file        pwm.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       LED PWM driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#include "pwm.h"

/**
 * @brief       LED PWMinitializationfunction
 * @param       frequency: PWMOutput frequency，unitHZ
 * @param       resolution: PWMDuty cycle resolution1-16，For example, setting8，Resolution range0~255
 * @retval      none
 */
void led_pwm_init(uint16_t frequency, uint8_t resolution)
{
    ledcSetup(LED_PWM_CHANNEL, frequency, resolution);    /* PWMinitialization, pin andChannel consist ofpwm.hofLED_PWM_PINandLED_PWM_CHANNELMacro modification */
    ledcAttachPin(LED_PWM_PIN, LED_PWM_CHANNEL);          /* Bind the PWM channel to LED_PWM_PIN */
}

/**
 * @brief       PWMDuty Cycleset up
 * @param       duty: PWMDuty Cycle
 * @retval      none
 */
void pwm_set_duty(uint16_t duty)
{
    ledcWrite(LED_PWM_CHANNEL, duty);   /* Change the duty cycle of PWM, and the channel is modified by the LED_PWM_CHANNEL macro of pwm.h */
}
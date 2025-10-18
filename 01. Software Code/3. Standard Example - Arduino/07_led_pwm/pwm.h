/**
 ****************************************************************************************************
 * @file        pwm.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       LED PWM Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#ifndef __PWM_H
#define __PWM_H

#include "Arduino.h"

/* LED PWM definition */
#define LED_PWM_PIN       1   /* PWM signal output pin  */
#define LED_PWM_CHANNEL   0   /* LED PWM channel number */

/* function declaration */    
void led_pwm_init(uint16_t frequency, uint8_t resolution);    /* LED PWMInitialize function */
void pwm_set_duty(uint16_t duty);                             /* PWM duty cycle settings */

#endif
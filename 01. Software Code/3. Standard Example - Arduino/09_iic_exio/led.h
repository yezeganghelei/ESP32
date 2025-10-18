/**
 ****************************************************************************************************
 * @file        led.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       LED Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20230805
 * First release

 ****************************************************************************************************
 */

#ifndef __LED_H
#define __LED_H

#include "Arduino.h"

/* Pin definition */
#define LED_PIN       1   /* The LED on the development board is connected to the GPIO1 pin */

/* Macro function definition */
#define LED(x)        digitalWrite(LED_PIN, x)
#define LED_TOGGLE()  digitalWrite(LED_PIN, !digitalRead(LED_PIN))

/* function declaration */
void led_init(void);      /* LED pin initialization function */

#endif
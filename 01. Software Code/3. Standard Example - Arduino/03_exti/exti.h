/**
 ****************************************************************************************************
 * @file        exti.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       EXTI driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#ifndef __EXTI_H
#define __EXTI_H

#include "Arduino.h"

extern uint8_t led_state;

/* Pin definition */
#define KEY_INT_PIN     0   /* External interrupt pin GPIO0 */

/* function declaration */
void exti_init(void);       /* External interrupt initialization function */
void key_isr(void);         /* KEY external interrupt callback function */

#endif
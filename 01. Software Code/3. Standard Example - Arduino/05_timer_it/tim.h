/**
 ****************************************************************************************************
 * @file        tim.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       TIM Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#ifndef __TIM_H
#define __TIM_H

#include "Arduino.h"

/* timer interrupt definition */
#define TIMx_INT        0
#define TIMx_ISR        tim0_ISR

/* Function declaration */
void timx_int_init(uint16_t arr, uint16_t psc);     /* Timer interrupt initialization function */
void TIMx_ISR(void);                                /* Timer interrupt callback function */

#endif
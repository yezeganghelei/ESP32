/**
 ****************************************************************************************************
 * @file        adc.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       ADC driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#ifndef __ADC_H
#define __ADC_H

#include "Arduino.h"

/* Pin definition */
#define ADC_IN_PIN      8        

/* function declaration */
uint16_t adc_get(uint8_t adc_pin);   /* adc get function */

#endif
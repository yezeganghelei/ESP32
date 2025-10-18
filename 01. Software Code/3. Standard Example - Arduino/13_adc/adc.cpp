/**
 ****************************************************************************************************
 * @file        adc.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       ADC driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#include "adc.h"

/**
* @brief       getADCvalue function
* @param       adc_pin：adcpin
* @retval      getArrivedADCvalue
*/
uint16_t adc_get(uint8_t adc_pin) 
{
    return analogRead(adc_pin);   /* returnADCvalue */
}
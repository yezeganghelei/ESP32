/**
 ****************************************************************************************************
 * @file        led.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       LED Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#include "led.h"

/**
* @brief       initializationLEDRelatedIOmouth
* @param       none
* @retval      none
*/
void led_init(void) 
{
    pinMode(LED_PIN, OUTPUT);     /* set upledThe pin is in output mode */
    digitalWrite(LED_PIN, HIGH);  /* Combined with schematic design,Real thingsLEDGetting high will turn off */
}
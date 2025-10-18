/**
 ****************************************************************************************************
 * @file        exti.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       EXTI driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#include "exti.h"
#include "key.h"

uint8_t led_state = 0;        /* Determine the light on and off stateChangequantity */

/**
* @brief       initializationExternal interrupt relatedIOmouth
* @param       none
* @retval      none
*/
void exti_init(void) 
{
    key_init();                                                             /* KEYinitialization */
    attachInterrupt(digitalPinToInterrupt(KEY_INT_PIN), key_isr, FALLING);  /* Set the KEY pin as an interrupt pin and trigger on the falling edge */
}

/**
 * @brief      KEYExternal interrupt callback function
 * @param      none
 * @retval     none
 */
void key_isr(void)
{
    delay(10);
    if (KEY == 0)
    {
        led_state =! led_state;     /* Two situations：from0Change1，from1Changefor0 */
    }
}
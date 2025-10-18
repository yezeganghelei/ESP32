/**
 ****************************************************************************************************
 * @file        key.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       KEY Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#include "key.h"

/**
* @brief Initialize KEY-related IO port
* @param None
* @retval None
*/
void key_init(void) 
{
    /* Design in combination with schematic diagram,When the button is not pressed,KEYThe pin detects a high level */
    pinMode(KEY_PIN, INPUT_PULLUP);    /* set upkeyPin is pull-up input mode */
}
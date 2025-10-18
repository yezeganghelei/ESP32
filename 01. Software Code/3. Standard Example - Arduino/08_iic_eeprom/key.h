/**
 ****************************************************************************************************
 * @file        key.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       KEY driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#ifndef __KEY_H
#define __KEY_H

#include "Arduino.h"

/* Pin definition */
#define KEY_PIN       0   /* KEY on the development board is connected to the GPIO0 pin */

/* Macro function definition */
#define KEY           digitalRead(KEY_PIN)    /* readKEYPin status */

/* function declaration */
void key_init(void);      /* keyPin initialization function */

#endif
/**
 ****************************************************************************************************
 * @file        key.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       KEY Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#ifndef __KEY_H
#define __KEY_H

#include "Arduino.h"

/* Pin definition */
#define KEY_PIN       0   /* Connect KEY on the development board to GPIO0 pin */

/* Macro function definition */
#define KEY           digitalRead(KEY_PIN)    /* ReadKEYPin status */

/* function declaration */
void key_init(void);      /* keyPin initialization function */

#endif
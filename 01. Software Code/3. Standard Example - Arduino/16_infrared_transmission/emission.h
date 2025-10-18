/**
 ****************************************************************************************************
 * @file        emission.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       EMISSION Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#ifndef __EMISSION_H
#define __EMISSION_H

#include "Arduino.h"

/* Pin definition */
#define RMT_TX_PIN 8
 
/* function declaration */
void emission_init(void);
void emission_send(uint8_t data);

#endif
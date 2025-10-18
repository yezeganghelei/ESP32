/**
 ****************************************************************************************************
 * @file        remote.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       REMOTE Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#ifndef __REMOTE_H
#define __REMOTE_H

#include "Arduino.h"

/* Pin definition */
#define RMT_RX_PIN 2

/* Function declaration */
void remote_init(void);         /* Infrared remote control initialization */
uint8_t remote_scan(void);      /* Handle infrared buttons */

#endif
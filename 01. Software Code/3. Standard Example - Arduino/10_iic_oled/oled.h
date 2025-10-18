/**
 ****************************************************************************************************
 * @file        oled.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       OLED Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#ifndef __OLED_H
#define __OLED_H

#include "Arduino.h"

/* Pin definition */
#define OLED_SCL_PIN      4
#define OLED_SDA_PIN      5
#define OLED_D2_PIN       6
#define OLED_DC_PIN       38

#define OLED_ADDR         0X3C  /* 7Bit device address */

#define OLED_RST(x)       xl9555_pin_set(OV_RESET, x ? IO_SET_HIGH : IO_SET_LOW)

/* Function declaration */
void oled_init(void);           /* OLEDModule initialization */
void oled_show_demo(void);      /* OLED module display effect */

#endif
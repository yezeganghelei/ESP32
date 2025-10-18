/**
 ****************************************************************************************************
 * @file        24c02.h
 * @author      
 * @version     V1.0
 * @date        2023-08-18
 * @brief       24C02 Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20230818
 * first release

 ****************************************************************************************************
 */

#ifndef __AP3216C_H
#define __AP3216C_H

#include "Arduino.h"

/* Pin definition */
#define IIC_SCL         42
#define IIC_SDA         41

#define AP3216C_ADDR    0X1E    /* 7bit device address */

/* function declaration */
uint8_t ap3216c_init(void);                                         /* AP3216C initialization function */
void ap3216c_write_one_byte(uint8_t reg, uint8_t data);             /* WriteAP3216CRegister function */
uint8_t ap3216c_read_one_byte(uint8_t reg);                         /* readAP3216CRegister data function */
void ap3216c_read_data(uint16_t *ir, uint16_t *ps, uint16_t *als);  /* readAP3216Cdata */

#endif
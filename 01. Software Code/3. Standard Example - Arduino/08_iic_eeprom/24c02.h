/**
 ****************************************************************************************************
 * @file        24c02.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       24C02 Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#ifndef __24C02_H
#define __24C02_H

#include "Arduino.h"

/* Pin definition */
#define IIC_SCL     42
#define IIC_SDA     41

#define EEPROM_ADDR 0x50    /* 7bit device address */

/* function declaration */
void at24c02_init(void);                                            /* Initialize IIC */
uint8_t at24c02_check(void);                                        /* Check the device */
uint8_t at24c02_read_one_byte(uint8_t addr);                        /* Read a byte from the specified address */
void at24c02_write_one_byte(uint8_t addr,uint8_t data);             /* Write a byte to the specified address */
void at24c02_write(uint8_t addr, uint8_t *pbuf, uint8_t datalen);   /* Write data of specified length starting from specified address */
void at24c02_read(uint8_t addr, uint8_t *pbuf, uint8_t datalen);    /* Read data of specified length starting from specified address */

#endif
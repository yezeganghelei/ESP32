/**
 ****************************************************************************************************
 * @file        uart.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       UART Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#ifndef __UART_H
#define __UART_H

#include "Arduino.h"

/* Pin definition */
/* stringmouth0Fixed is already used by defaultIO(GPIO43forU0TXD,GPIO44forU0RXD)  
 * The following two macrosforstringmouth1orstringmouth2UsedIOmouth(Routine not used)
 */
#define TXD_PIN      19
#define RXD_PIN      20

/* function declaration */
void uart_init(uint8_t uartx, uint32_t baud);   /* uart initialization function */

#endif
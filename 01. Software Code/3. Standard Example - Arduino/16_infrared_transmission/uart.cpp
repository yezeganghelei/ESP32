/**
 ****************************************************************************************************
 * @file        uart.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       UART Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#include "uart.h"

/**
* @brief       initializationUART
* @param       uartx：Serial portx
* @param       baud：baud rate
* @retval      none
*/
void uart_init(uint8_t uartx, uint32_t baud) 
{
    if (uartx == 0)
    {
        Serial.begin(baud);                                   /* Serial port0initialization */
    }
    else if (uartx == 1)
    {
        Serial1.begin(baud, SERIAL_8N1, RXD_PIN, TXD_PIN);    /* Serial port1initialization */
    }
    else if (uartx == 2)
    {
        Serial2.begin(baud, SERIAL_8N1, RXD_PIN, TXD_PIN);    /* Serial port2initialization */
    }
}
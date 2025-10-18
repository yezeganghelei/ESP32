/**
 ****************************************************************************************************
 * @file        emission.cpp
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

#include "emission.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "spilcd.h"

IRsend irsend(RMT_TX_PIN);

/**
 * @brief       Infrared send initialization
 * @param       none
 * @retval      none
 */
void emission_init(void)
{
    irsend.begin();
}

/**
 * @brief       Send infrared data
 * @param       data:control code
 * @retval      none
 */
void emission_send(uint8_t data)
{  
    uint32_t send_data = 0;
    uint8_t data_oppo = 0;

    data_oppo = ~data;
    send_data = 0x00FF0000UL | data << 8 | data_oppo;
    irsend.sendNEC(send_data);
}
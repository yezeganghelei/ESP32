/**
 ****************************************************************************************************
 * @file        remote.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       REMOTE driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#include "remote.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "spilcd.h"

IRrecv irrecv(RMT_RX_PIN);
decode_results results;

/**
 * @brief infrared remote control initialization
 * @param none
 * @retval None
 */
void remote_init(void)
{
    irrecv.enableIRIn();
}

/**
 * @brief       Handle infrared keys
 * @param       none
 * @retval      0:No keys pressed
 *              other:The key value of the pressed 
 */
uint8_t remote_scan(void)
{  
    uint8_t rmt_data = 0;         /* Infrared remote control key value */
    uint8_t remote_key = 0;       /* Extract control code */
    uint32_t raw_data = 0;        /* raw data */

    if (irrecv.decode(&results))  /* Decode infrared data */
    {
        raw_data = results.value;
        remote_key = (uint8_t)(raw_data >> 8);          /* Extract control code */
        
        if ((remote_key != 0xFF) && (remote_key != 0))  /* Prove that valid key values ​​have been obtained */
        {
            rmt_data = remote_key;
        }

        irrecv.resume();          /* receive next value */
        
        return rmt_data;
    }
    
    return 0;
}
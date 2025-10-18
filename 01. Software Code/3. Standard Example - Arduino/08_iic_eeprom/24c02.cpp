/**
 ****************************************************************************************************
 * @file        24c02.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       24C02 driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#include "24c02.h"
#include <Wire.h>

/**
 * @brief Initialize the EEPROM device
 * @param None
 * @retval None
 */
void at24c02_init(void)
{
    Wire.begin(IIC_SDA, IIC_SCL, 400000);    /* Initialize IIC connection */
}

/**
 * @brief       existAT24C02Read a data with a specified address
 * @param       addr: The address to start reading data
 * @retval      Read data / 0xFF:No data received
 */
uint8_t at24c02_read_one_byte(uint8_t addr)
{
    Wire.beginTransmission(EEPROM_ADDR);      /* sendFrom the machine7BitDevicesaddress tosendqueue */
    Wire.write(addr);                         /* Send the address to read slave data to the send queue */
    Wire.endTransmission(0);                  /* IIC send Send the queue data(Transfer to the0,Indicates resending onestartSignal,KeepIICbus validconnect) */

    Wire.requestFrom(EEPROM_ADDR, 1);         /* The host sends data request to the slave, And get the data */
    if (Wire.available() != 0)                /* Get the number of data bytes that have been received */
    {
        return Wire.read();                   /* Read data from the data buffer */
    }

    return 0xFF;
}

/**
 * @brief       existAT24C02Write a data to the specified address
 * @param       addr: Destination address for writing data
 * @param       data: data to write
 * @retval      none
 */
void at24c02_write_one_byte(uint8_t addr, uint8_t data)
{
    Wire.beginTransmission(EEPROM_ADDR);    /* sendFrom the machine7BitDevicesaddress tosendqueue */
    Wire.write(addr);                       /* send the address to write slave data to the send queue */
    Wire.write(data);                       /* Send data to be written to the slave to the send queue */
    Wire.endTransmission(1);                /* IIC send Send the queue data(Transfer to the1,expresssendstopSignal,end transfer) */

    delay(10);    /* Notice: EEPROM Writing is slow,have to wait10msand then write the next byte */
}

/**
 * @brief       examineAT24C02Is it normal
 *   @note      Detection principle: existDevicesThe last address is written as0X55, and then read, If the value is readfor0X55
 *              It means the detection is normal. otherwise,It means that the detection failed.

 * @param       none
 * @retval      Test results
 *              0: Detection successful
 *              1: Detection failed
 */
uint8_t at24c02_check(void)
{
    uint8_t temp;

    temp = at24c02_read_one_byte(255);      /* Avoid writing every time you turn on the computerAT24CXX */

    if (temp == 0X55)                       /* Reading data is normal */
    {
        return 0;
    }
    else                                    /* exclude first timeinitializationsituation */
    {
        at24c02_write_one_byte(255, 0X55);  /* Write data first */
        temp = at24c02_read_one_byte(255);  /* Read data again */
        
        if (temp == 0X55) 
        {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief       existAT24C02Start reading the specified number of data from the specified address inside
 * @param       addr    : Address to start reading from right24c02for0~255
 * @param       pbuf    : Data array first address
 * @param       datalen : Number of data to be read
 * @retval      none
 */
void at24c02_read(uint8_t addr, uint8_t *pbuf, uint8_t datalen)
{
    while (datalen--)
    {
        *pbuf++ = at24c02_read_one_byte(addr++);
    }
}

/**
 * @brief       existAT24C02The specified address starts writing the specified number of data
 * @param       addr    : The address to start writing right24c02for0~255
 * @param       pbuf    : Data array first address
 * @param       datalen : The number of data to be written
 * @retval      none
 */
void at24c02_write(uint8_t addr, uint8_t *pbuf, uint8_t datalen)
{
    while (datalen--)
    {
        at24c02_write_one_byte(addr, *pbuf);
        addr++;
        pbuf++;
    }
}
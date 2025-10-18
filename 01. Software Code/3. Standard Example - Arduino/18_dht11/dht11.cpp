/**
 ****************************************************************************************************
 * @file        dht11.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       DHT11 driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#include "dht11.h"

/**
* @brief       resetDHT11
* @param       none
* @retval      none       
*/
void dht11_reset(void) 
{
    DHT11_MODE_OUT;         /* IOMode set to output */
    DHT11_DQ_OUT(0);        /* Lower DQ */
    delay(20);              /* Pull up at least18ms */
    DHT11_DQ_OUT(1);        /* Pull upDQ */
    delayMicroseconds(30);  /* HostPull up10~35us */
}

/**
* @brief       waitDHT11Response
* @param       none
* @retval      0：normal，1：Does not exist/Abnormal      
*/
uint8_t dht11_check(void) 
{
    uint8_t retry = 0;
    uint8_t rval = 0;

    DHT11_MODE_IN;                        /* IOMode set to input */

    while (DHT11_DQ_IN && retry < 100)    /* DHT11Will lower the approximate83us */
    {
        retry++;
        delayMicroseconds(1);
    }

    if (retry >= 100)
    {
        rval = 1;
    }
    else
    {
        retry = 0;

        while (!DHT11_DQ_IN && retry < 100) /* DHT11 will be pushed up again by about 87us */
        {
            retry++;
            delayMicroseconds(1);
        }

        if (retry >= 100) 
        {
            rval = 1;
        }
    }
    
    return rval;
}

/**
 * @brief       fromDHT11Readone bit
 * @param       none
 * @retval      Readplace value: 0 / 1
 */
uint8_t dht11_read_bit(void)
{
    uint8_t retry = 0;

    while (DHT11_DQ_IN && retry < 100)  /* waitgoes low */
    {
        retry++;
        delayMicroseconds(1);
    }

    retry = 0;

    while (!DHT11_DQ_IN && retry < 100) /* wait goes high */
    {
        retry++;
        delayMicroseconds(1);
    }

    delayMicroseconds(40);              /* Wait for 40us */

    if (DHT11_DQ_IN)                    /* Return bit according to pin status */
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}

/**
 * @brief       fromDHT11Readone byte
 * @param       none
 * @retval      readdata
 */
uint8_t dht11_read_byte(void)
{
    uint8_t i, data = 0;

    for (i = 0; i < 8; i++)         /* Loop to read 8-bit data */
    {
        data <<= 1;                 /* High-order data is output first, Move one position to the left first */
        data |= dht11_read_bit();   /* Read1bitdata */
    }

    return data;
}

/**
 * @brief       fromDHT11Readoncedata
 * @param       temp: Temperature value(scope:-20~60°)
 * @param       humi: Humidity value(scope:5%~95%)
 * @retval      0, normal.
 *              1, fail
 */
uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;
    
    dht11_reset();

    if (dht11_check() == 0)
    {
        for (i = 0; i < 5; i++)     /* Read 40 bits of data */
        {
            buf[i] = dht11_read_byte();
        }

        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
    }
    else
    {
        return 1;
    }
    
    return 0;
}

/**
* @brief       initializationDHT11
* @param       none
* @retval      0：normal，1：Does not exist/Abnormal
*/
uint8_t dht11_init(void) 
{
    dht11_reset();
    return dht11_check();
}
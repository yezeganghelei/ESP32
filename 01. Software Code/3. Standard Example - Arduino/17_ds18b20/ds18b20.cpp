/**
 ****************************************************************************************************
 * @file        ds18b20.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       DS18B20 Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#include "ds18b20.h"

/**
 * @brief       ResetDS18B20
 * @param       none
 * @retval      none
 */
static void ds18b20_reset(void)
{
    DS18B20_MODE_OUT;
    DS18B20_DQ_OUT(0);        /* Pull downDQ,Reset */
    delayMicroseconds(750);   /* Pull down 750us */
    DS18B20_DQ_OUT(1);        /* DQ=1, releaseReset */
    delayMicroseconds(15);    /* Delay 15US */
}

/**
 * @brief       waitDS18B20response
 * @param       none
 * @retval      0, DS18B20normal
 *              1, DS18B20abnormal/does not exist
 */
uint8_t ds18b20_check(void)
{
    uint8_t retry = 0;
    uint8_t rval = 0;

    DS18B20_MODE_IN;
    while (DS18B20_DQ_IN && retry < 200)    /* waitDQGo low, wait200us */
    {
        retry++;
        delayMicroseconds(1);
    }

    if (retry >= 240)
    {
        rval = 1;
    }
    else
    {
        retry = 0;

        while (!DS18B20_DQ_IN && retry < 240)   /* waitDQget taller, wait240us */
        {
            retry++;
            delayMicroseconds(1);
        }

        if (retry >= 240)
        {
            rval = 1;
        }
    }

    return rval;
}

/**
 * @brief       fromDS18B20read a bit
 * @param       none
 * @retval      bit value read: 0 / 1
 */
static uint8_t ds18b20_read_bit(void)
{
    uint8_t data = 0;

    DS18B20_MODE_OUT;
    DS18B20_DQ_OUT(0);
    delayMicroseconds(2);
    DS18B20_DQ_OUT(1);
    delayMicroseconds(12);

    DS18B20_MODE_IN;
    if (DS18B20_DQ_IN)
    {
        data = 1;
    }

    delayMicroseconds(50);
    return data;
}

/**
 * @brief       fromDS18B20Read a byte
 * @param       none
 * @retval      data read
 */
static uint8_t ds18b20_read_byte(void)
{
    uint8_t i, b, data = 0;

    for (i = 0; i < 8; i++)
    {
        b = ds18b20_read_bit(); /* DS18B20 outputs low-order data first, then high-order data. */
        
        data |= b << i;         /* fillingdataEvery one of */ 
    }

    return data;
}

/**
 * @brief       Write a byte toDS18B20
 * @param       data: Bytes to be written
 * @retval      none
 */
static void ds18b20_write_byte(uint8_t data)
{
    uint8_t j;

    DS18B20_MODE_OUT;

    for (j = 1; j <= 8; j++)
    {
        if (data & 0x01)
        {
            DS18B20_DQ_OUT(0);  
            delayMicroseconds(2);
            DS18B20_DQ_OUT(1);
            delayMicroseconds(60);
        }
        else
        {
            DS18B20_DQ_OUT(0);  
            delayMicroseconds(60);
            DS18B20_DQ_OUT(1);
            delayMicroseconds(2);
        }

        data >>= 1;             
    }
}

/**
 * @brief Start the temperature conversion
 * @param None
 * @retval None
 */
static void ds18b20_start(void)
{
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc);   /*  skip rom */
    ds18b20_write_byte(0x44);   /*  convert */
}

/**
* @brief       initializationDS18B20
* @param       none
* @retval      0：normal，1：does not exist/Not normal
*/
uint8_t ds18b20_init(void) 
{
    ds18b20_reset();
    return ds18b20_check();
}

/**
 * @brief       fromds18b20gettemperature value(Accuracy：0.1C)
 * @param       none
 * @retval      temperature value （-550~1250）
 *   @note      returnedtemperature valueenlarged10times.
 *              When actually used,to divide by10is the actual temperature.
 */
short ds18b20_get_temperature(void)
{
    uint8_t flag = 1;           /* The default temperature is a positive number */
    uint8_t TL, TH;
    short temp;

    ds18b20_start();            /*  ds1820 start convert */
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc);   /*  skip rom */
    ds18b20_write_byte(0xbe);   /*  convert */
    TL = ds18b20_read_byte();   /*  LSB */
    TH = ds18b20_read_byte();   /*  MSB */

    if (TH > 7)
    { /* Temperature is negative，CheckDS18B20The temperature representation is consistent with the principle of computer storing positive and negative data：
        The positive complement is the data stored in the register itself，The complement of a negative number is the bitwise inversion of the register storage value.+1
        So we take the actual negative part of it，But the complement of a negative number is negation and then plus one.，But considering that the low position may+1There are carry and code redundancy afterwards，
        We haven't done it yet+1processing，Need to pay attention here	*/
        TH = ~TH;
        TL = ~TL;
        flag = 0;   
    }

    temp = TH;      /* Get the top eight */
    temp <<= 8;
    temp += TL;     /* Get the bottom eight */

    /* Convert to actual temperature */
    if (flag == 0)
    { /* Convert the temperature to a negative temperature. The +1 here refers to the previous instructions. */
        temp = (double)(temp + 1) * 0.625;
        temp = -temp;   
    }
    else
    {
        temp = (double)temp * 0.625;				
    }

    return temp;
}
/**
 ****************************************************************************************************
 * @file        ap3216c.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       AP3216C Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20230818
 * first release

 ****************************************************************************************************
 */

#include "ap3216c.h"
#include <Wire.h>

/**
 * @brief       initializationlight environment sensor
 * @param       none
 * @retval      0:initializationsuccess；1:initializationfail
 */
uint8_t ap3216c_init(void)
{
    uint8_t temp;

    Wire.begin(IIC_SDA, IIC_SCL, 400000);   /* Initialize IIC connection */

    ap3216c_write_one_byte(0x00, 0X04);     /* Reset AP3216C */
    delay(50);                              /* AP3216C reset at least 10ms */

    ap3216c_write_one_byte(0x00, 0X03);     /* OpenALS、PS+IR */

    temp = ap3216c_read_one_byte(0X00);     /* Read what was just written0X03 */
    if (temp == 0X03)
    {
        return 0;           /* AP3216C is normal */
    }
    else 
    {
        return 1;           /* AP3216C failed */
    }
}

/**
 * @brief       Towardsap3216cWrite a data to the specified register
 * @param       reg: Register to be written
 * @param       data: wantwritedata
 * @retval      none
 */
void ap3216c_write_one_byte(uint8_t reg, uint8_t data)
{
    Wire.beginTransmission(AP3216C_ADDR);     /* 7bit device address tosendqueue of sendSlave machine */
    Wire.write(reg);                          /* sendwantwriteSlave machineregister address tosendqueue */
    Wire.write(data);                         /* Send data to be written to the slave register to the send queue */
    Wire.endTransmission();                   /* IIC send sendqueuedata(without parameters,expresssendstopSignal,end transfer) */         
}

/**
 * @brief       existap3216cSpecify the register to read a data
 * @param       reg: register to read
 * @retval      Register value / 0xFF:No data received
 */
uint8_t ap3216c_read_one_byte(uint8_t reg)
{
    uint8_t rd_num = 0;
    uint8_t rd_data = 0;

    Wire.beginTransmission(AP3216C_ADDR);       /* 7bit device address tosendqueue of sendSlave machine */
    Wire.write(reg);                            /* sendwantreadslave register address tosendqueue */
    Wire.endTransmission(0);                    /* IIC send send queue data (Pass parameters as0, indicating re-send a startSignal, KeepIICbus active connection) */

    rd_num = Wire.requestFrom(AP3216C_ADDR, 1); /* The host Towards the slave send data request, And get the data */

    if (Wire.available() != 0)                  /* Get the number of data bytes that have been received */
    {
        return Wire.read();                     /* Read data into the data buffer */
    }

    return 0xFF;
}

/**
 * @brief readAP3216Cdata
 * @note read original data, includeALS, PSandIR
 * If open at the same timeALS, IR+PS, the time interval between two data reads should be greater than 112.5ms
 * @param ir: IRSensor value
 * @param ps: PSSensor value
 * @param als: ALSSensor value
 * @retval none
 */
void ap3216c_read_data(uint16_t *ir, uint16_t *ps, uint16_t *als)
{
    uint8_t buf[6];
    uint8_t i;

    for (i = 0; i < 6; i++)
    {
        buf[i] = ap3216c_read_one_byte(0X0A + i);   /* Loop to read all sensor data */
    }

    if (buf[0] & 0X80)
    {
        *ir = 0;                                                    /* If the IR_OF bit is 1, the data is invalid */
    }
    else 
    {
        *ir = ((uint16_t)buf[1] << 2) | (buf[0] & 0X03);            /* readIRsensordata */
    }

    *als = ((uint16_t)buf[3] << 8) | buf[2];                        /* readALSsensordata */ 

    if (buf[4] & 0x40) 
    {
        *ps = 0;                                                    /* If the IR_OF bit is 1, the data is invalid */
    }
    else
    {
        *ps = ((uint16_t)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F);   /* readPSsensordata */
    }
}
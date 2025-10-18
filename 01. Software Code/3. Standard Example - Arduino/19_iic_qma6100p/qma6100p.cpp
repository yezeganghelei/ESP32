/**
 ****************************************************************************************************
 * @file        qma6100p.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       QMA6100P driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#include "qma6100p.h"
#include <Wire.h>

#define M_G     9.80665f

/**
 * @brief QMA6100P register configuration function
 * @param reg: register address
 * @param data: data written to the register
 * @retval None
 */
void qma6100p_write_reg(uint8_t reg, uint8_t data)
{
    Wire.beginTransmission(IMU_DEV_ADDR);     /* sendfrommachineof7BitDevicesaddressarrivesendqueue */
    Wire.write(reg);                          /* sendwant writes addressarrivesendqueue frommachineregister */
    Wire.write(data);                         /* sendwant writes dataarrivesendqueue frommachineregister */
    Wire.endTransmission();                   /* IIC send sendqueue data (Without parameters, expresssendstopSignal, end transfer) */
}

/**
 * @brief       fromQMA6100PReadNBytes of data
 * @param       reg  : registeraddress
 * @param       buf  : data storagebuf
 * @param       num   : Read length
 * @retval      none
 */
void qma6100p_read_reg(uint8_t reg, uint8_t *buf, uint16_t num)
{
    uint8_t i = 0;

    Wire.beginTransmission(IMU_DEV_ADDR);     /* sendfrommachineof7BitDevicesaddressarrivesendqueue */
    Wire.write(reg);                          /* sendwantReadfrommachine registeraddressarrivesendqueue */
    Wire.endTransmission(0);                  /* IIC send sendqueueofdata(Transfer to the0,expressagainsendonestartSignal,KeepIICBus active connection) */

    Wire.requestFrom(IMU_DEV_ADDR, num);      /* host tofrommachinesenddata request,and getarrivedata */

    while (Wire.available())                  /* Get the number of data bytes that have been received */
    {
        buf[i++] = Wire.read();               /* arrivedatabufferReaddata */
    }
}

/**
* @brief qma6100pinitialization function
* @param none
* @retval 0:success，No0:fail
*/
uint8_t qma6100p_init(void)
{
    uint8_t id = 0;

    Wire.begin(IIC_SDA, IIC_SCL, 400000);           /* initializationIICbus */

    qma6100p_read_reg(QMA6100P_CHIP_ID, &id, 1);    /* ReadQMA6100PDevicesID */
    if (id != QMA6100P_DEVICE_ID)
    {
        Serial.printf("ID:%#x \r\n", id);
        return 0xFF;
    }

    /* software reset */
    qma6100p_write_reg(QMA6100P_REG_RESET, 0xb6);
    delay(5);
    qma6100p_write_reg(QMA6100P_REG_RESET, 0x00);
    delay(10);

    /* Recommended by the manufacturerinitializationsequence */
    qma6100p_write_reg(0x11, 0x80);   /* fromstandbyarriveactivestate(0x11register<7>=1) */
    qma6100p_write_reg(0x11, 0x84);
    qma6100p_write_reg(0x4a, 0x20);
    qma6100p_write_reg(0x56, 0x01);
    qma6100p_write_reg(0x5f, 0x80);
    delay(2);
    qma6100p_write_reg(0x5f, 0x00);
    delay(10);

    /* set upQMA6100PConfiguration parameters(Range、Output frequency、working mode) */
    qma6100p_write_reg(QMA6100P_REG_RANGE, QMA6100P_RANGE_8G);                              /* set upaccelerometer fullRangescope(8G) */
    qma6100p_write_reg(QMA6100P_REG_BW_ODR, (uint8_t)(QMA6100P_BW_100 | QMA6100P_LPF_OFF)); /* set upoutputdatarate(100khz) */
    qma6100p_write_reg(QMA6100P_REG_POWER_MANAGE, (uint8_t)QMA6100P_MCLK_51_2K | 0x80);     /* set up working mode of acceleration */

    return 0;
}

/**
 * @brief       fromQMA6100PregistermiddleReadoriginalx,y,zAxis data
 * @param       data  : 3axis data storage array
 * @retval      none
 */
void qma6100p_read_raw_xyz(int16_t data[3])
{
    uint8_t databuf[6] = {0}; 
    int16_t raw_data[3];

    qma6100p_read_reg(QMA6100P_XOUTL, databuf, 6);

    raw_data[0] = (int16_t)(((databuf[1] << 8)) | (databuf[0]));
    raw_data[1] = (int16_t)(((databuf[3] << 8)) | (databuf[2]));
    raw_data[2] = (int16_t)(((databuf[5] << 8)) | (databuf[4]));

    data[0] = raw_data[0] >> 2;
    data[1] = raw_data[1] >> 2;
    data[2] = raw_data[2] >> 2;
}

/**
 * @brief Calculate the x, y, z axis data of the accelerometer
 * @param accdata: 3-axis data storage array
 * @retval None
 */
void qma6100p_read_acc_xyz(float accdata[3]) 
{
    int16_t rawdata[3];

    qma6100p_read_raw_xyz(rawdata);

    accdata[0] = (float)(rawdata[0] * M_G) / 1024;
    accdata[1] = (float)(rawdata[1] * M_G) / 1024;
    accdata[2] = (float)(rawdata[2] * M_G) / 1024;
}
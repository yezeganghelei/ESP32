/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file 24cxx.c
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief 24CXX driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "24cxx.h"

i2c_obj_t at24cxx_master;

/**
 * @brief       Initialize the IIC interface
 * @param       i2c_obj_t self: Incoming IIC initialization parameter, used to determine whether IIC initialization has been completed
 * @retval      none
 */
void at24cxx_init(i2c_obj_t self)
{
    at24cxx_master = self;

    if (self.init_flag == ESP_FAIL)
    {
        iic_init(I2C_NUM_0);        /* initializationIIC */
    }
}

/**
 * @brief       Read one byte at the specified address from AT24CXX
 * @param       addr: Address to start reading from
 * @retval      Read data
 */
uint8_t at24cxx_read_one_byte(uint16_t addr)
{
    uint8_t data = 0;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    /* Send the high address according to the different 24CXX model
     * 1. For 24C16 and above, send the address in 2 bytes
     * 2. For 24C16 and below, send 1 low byte address + the device address bits 1~3 are used to represent the high address bits, up to 11 address bits
     *    For 24C01/02, its device address format (8 bits) is: 1  0  1  0  A2  A1  A0  R/W
     *    For 24C04,    its device address format (8 bits) is: 1  0  1  0  A2  A1  a8  R/W
     *    For 24C08,    its device address format (8 bits) is: 1  0  1  0  A2  a9  a8  R/W
     *    For 24C16,    its device address format (8 bits) is: 1  0  1  0  a10 a9  a8  R/W
     *    R/W      : read/write control bit, 0 = write; 1 = read;
     *    A0/A1/A2 : correspond to pins 1, 2, 3 of the device (only 24C01/02/04/8 have these pins)
     *    a8/a9/a10: correspond to the high address bits of the storage array; 11 address bits can represent 2048 bits, enough to address 24C16 models
     */
    if(EE_TYPE > AT24C16)
    {
        i2c_master_write_byte(cmd, (AT_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);    /* Send write command */
        i2c_master_write_byte(cmd, addr >> 8, ACK_CHECK_EN);                            /* Send high address */
    }
    else
    {
        i2c_master_write_byte(cmd, 0XA0 + ((addr / 256) << 1), ACK_CHECK_EN);           /* Send device address0XA0,Write data */
    }
    i2c_master_write_byte(cmd, addr % 256, ACK_CHECK_EN);                               /* Send low address */
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AT_ADDR << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(at24cxx_master.port, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(10);

    return data;
}

/**
 * @brief       Write one byte to the specified address in AT24CXX
 * @param       addr: Destination address for writing data
 * @param       data: Data to be written
 * @retval      none
 */
void at24cxx_write_one_byte(uint16_t addr, uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
        
    if(EE_TYPE > AT24C16)
    {
        i2c_master_write_byte(cmd, (AT_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);    /* Send write command */
        i2c_master_write_byte(cmd, addr >> 8, ACK_CHECK_EN);                            /* Send high address */
    }
    else
    {
        i2c_master_write_byte(cmd, 0XA0 + ((addr/256) << 1), ACK_CHECK_EN);             /* Send device address0XA0,Write data */
    }

    i2c_master_write_byte(cmd, addr % 256, ACK_CHECK_EN);                               /* Send low address */
    i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(at24cxx_master.port, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(10);
}

/**
 * @brief       Check whether AT24CXX is normal
 * @note        Detection principle: write 0X55 to the last address of the device, then read it back; if the read value is 0X55
 *              it means the detection is normal, otherwise it indicates detection failed.
 * @param       none
 * @retval      Test results
 *              0: Detection successful
 *              1: Detection failed
 */
uint8_t at24cxx_check(void)
{
    uint8_t temp;
    uint16_t addr = EE_TYPE;

    temp = at24cxx_read_one_byte(addr);     /* Avoid writing AT24CXX every time you turn on the computer */

    if (temp == 0X55)                       /* Reading data is normal */
    {
        return 0;
    }
    else                                    /* Exclude the case of first-time initialization */
    {
        at24cxx_write_one_byte(addr, 0X55); /* Write data first */
        temp = at24cxx_read_one_byte(255);  /* Read data again */

        if (temp == 0X55)
        {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief       Read the specified number of bytes starting from the specified address in AT24CXX
 * @param       addr    : The address to start reading right24c02for0~255
 * @param       pbuf    : Data array first address
 * @param       datalen : The number of data to be read out
 * @retval      none
 */
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        *pbuf++ = at24cxx_read_one_byte(addr++);
    }
}

/**
 * @brief       Write the specified number of bytes starting at the specified address in AT24CXX
 * @param       addr    : The address to start writing right24c02for0~255
 * @param       pbuf    : Data array first address
 * @param       datalen : The number of data to be written
 * @retval      none
 */
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        at24cxx_write_one_byte(addr, *pbuf);
        addr++;
        pbuf++;
    }
}
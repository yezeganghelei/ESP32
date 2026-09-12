/**
 ****************************************************************************************************
 * @file        24cxx.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       24CXX driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "24cxx.h"

i2c_obj_t at24cxx_master;

/**
 * @brief       Initialize IIC interface
 * @param       i2c_obj_t self: IIC initialization parameters, used to determine whether IIC initialization has been completed
 * @retval      none
 */
void at24cxx_init(i2c_obj_t self)
{
    at24cxx_master = self;

    if (self.init_flag == ESP_FAIL)
    {
        iic_init(I2C_NUM_0);        /* Initialize IIC */
    }
}

/**
 * @brief       Read one byte from the specified address of AT24CXX
 * @param       addr: Address to start reading
 * @retval      The data read
 */
uint8_t at24cxx_read_one_byte(uint16_t addr)
{
    uint8_t data = 0;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    /* Depending on the 24CXX model, send the high address bits
     * 1. For models above 24C16, send the address as 2 bytes
     * 2. For 24C16 and below, send 1 low byte plus device address bits 1~3 to indicate the high address bits (up to 11 address bits)
     *    for 24C01/02, the 8-bit device address format is: 1  0  1  0  A2  A1  A0  R/W
     *    for 24C04,    the 8-bit device address format is: 1  0  1  0  A2  A1  a8  R/W
     *    for 24C08,    the 8-bit device address format is: 1  0  1  0  A2  a9  a8  R/W
     *    for 24C16,    the 8-bit device address format is: 1  0  1  0  a10 a9  a8  R/W
     *    R/W      : Read/write control bit; 0 = write, 1 = read;
     *    A0/A1/A2 : Corresponding device pins 1, 2, 3 (only 24C01/02/04/08 have these pins)
     *    a8/a9/a10: High address bits; 11 address bits can address up to 2048 locations, covering 24C16 and smaller models
     */
    if(EE_TYPE > AT24C16)
    {
        i2c_master_write_byte(cmd, (AT_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);    /* Send a write command */
        i2c_master_write_byte(cmd, addr >> 8, ACK_CHECK_EN);                            /* Send high address */
    }
    else
    {
        i2c_master_write_byte(cmd, 0XA0 + ((addr / 256) << 1), ACK_CHECK_EN);           /* Send device address 0XA0, write data */
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
 * @brief       Write one byte to the specified address of AT24CXX
 * @param       addr: Destination address to write
 * @param       data: Data to be written
 * @retval      none
 */
void at24cxx_write_one_byte(uint16_t addr, uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
        
    if(EE_TYPE > AT24C16)
    {
        i2c_master_write_byte(cmd, (AT_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);    /* Send a write command */
        i2c_master_write_byte(cmd, addr >> 8, ACK_CHECK_EN);                            /* Send high address */
    }
    else
    {
        i2c_master_write_byte(cmd, 0XA0 + ((addr/256) << 1), ACK_CHECK_EN);             /* Send device address 0XA0, write data */
    }

    i2c_master_write_byte(cmd, addr % 256, ACK_CHECK_EN);                               /* Send low address */
    i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(at24cxx_master.port, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(10);
}

/**
 * @brief       Check whether AT24CXX is working properly
 * @note        Detection principle: write 0X55 to the last address of the device, then read it back; if the value read is 0X55
 *              it means the check passed; otherwise the check failed.
 * @param       none
 * @retval      Test results
 *              0: Test successfully
 *              1: Detection failed
 */
uint8_t at24cxx_check(void)
{
    uint8_t temp;
    uint16_t addr = EE_TYPE;

    temp = at24cxx_read_one_byte(addr);     /* Avoid writing AT24CXX every time you boot */

    if (temp == 0X55)                       /* Data read back is valid */
    {
        return 0;
    }
    else                                    /* Skip on first initialization */
    {
        at24cxx_write_one_byte(addr, 0X55); /* Write data first */
        temp = at24cxx_read_one_byte(255);  /* Read the data again */

        if (temp == 0X55)
        {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief       Read the specified number of bytes starting from the given address in AT24CXX
 * @param       addr    : Start address to read (0~255 for 24c02)
 * @param       pbuf    : Start address of the data array
 * @param       datalen : Number of bytes to read
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
 * @brief       Write the specified number of bytes starting from the given address in AT24CXX
 * @param       addr    : Start address to write (0~255 for 24c02)
 * @param       pbuf    : Start address of the data array
 * @param       datalen : Number of bytes to write
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
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
 * @brief       initializationIICinterface
 * @param       i2c_obj_t self: IncomingIICinitialization参数，Used to determine whether it has been completedIICinitialization
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
 * @brief       existAT24CXXRead a data with a specified address
 * @param       addr: Address to start reading from
 * @retval      Read data
 */
uint8_t at24cxx_read_one_byte(uint16_t addr)
{
    uint8_t data = 0;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    /* According to different24CXXmodel, Send high address
     * 1, 24C16The above models, point2Byte sending address
     * 2, 24C16and below models, point1low byte address + Occupies device addressbit1~bit3Bit 用于表示高Bit地址, most11Bit地址
     *    right于24C01/02, Its device address format(8bit)for: 1  0  1  0  A2  A1  A0  R/W
     *    right于24C04,    Its device address format(8bit)for: 1  0  1  0  A2  A1  a8  R/W
     *    right于24C08,    Its device address format(8bit)for: 1  0  1  0  A2  a9  a8  R/W
     *    right于24C16,    Its device address format(8bit)for: 1  0  1  0  a10 a9  a8  R/W
     *    R/W      : read/write control bit 0,Indicate writing; 1,Indicates reading;
     *    A0/A1/A2 : right应器件的1,2,3pin(only24C01/02/04/8have these feet)
     *    a8/a9/a10: right应存储整列的高Bit地址, 11bit地址most可以表示2048个Bit置,Can be addressed24C16Models within
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
 * @brief       existAT24CXXWrite a data to the specified address
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
 * @brief       examineAT24CXXIs it normal?
 * @note        Detection principle: exist器件的末地址写如0X55, and then read, 如果read取值for0X55
 *              It means the detection is normal. otherwise,则表示Detection failed.
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
    else                                    /* 排除第一次initialization的情况 */
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
 * @brief       existAT24CXXStart reading the specified number of data from the specified address inside
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
 * @brief       existAT24CXXThe specified address starts writing the specified number of data
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
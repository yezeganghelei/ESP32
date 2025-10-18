/**
 ****************************************************************************************************
 * @file        24cxx.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       24CXXDriver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "24cxx.h"

i2c_obj_t at24cxx_master;

/**
 * @brief       Initialize IICinterface
 * @param       i2c_obj_t self: IncomingIICInitialization parameters，Used to determine whether it has been completedIICinitialization
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
 * @brief       existAT24CXX指定addressreadOutput a data
 * @param       addr: Address to start reading
 * @retval      readThe data to be reached
 */
uint8_t at24cxx_read_one_byte(uint16_t addr)
{
    uint8_t data = 0;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    /* According to different24CXXmodel, send highBitaddress
     * 1, 24C16The abovemodel, point2Byte sending address
     * 2, 24C16and belowmodel, point1Low byte address + occupies device addressbit1~bit3Bit Used to indicate highBitaddress, most11Bitaddress
     *    for24C01/02, Its device address format(8bit)for: 1  0  1  0  A2  A1  A0  R/W
     *    for24C04,    Its device address format(8bit)for: 1  0  1  0  A2  A1  a8  R/W
     *    for24C08,    Its device address format(8bit)for: 1  0  1  0  A2  a9  a8  R/W
     *    for24C16,    Its device address format(8bit)for: 1  0  1  0  a10 a9  a8  R/W
     *    R/W      : read/Write control bit 0,Indicate writing; 1,expressread;
     *    A0/A1/A2 : Corresponding device1,2,3Pin(only24C01/02/04/8Have these feet)
     *    a8/a9/a10: rightThe height of the entire column should be storedBitaddress, 11bitThe address can be expressed at most2048Location,Can be addressed24C16and withinmodel
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
 * @brief       existAT24CXXWrite a data to the specified address
 * @param       addr: The destination address of the data written
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
 * @brief       examineAT24CXXIs it normal
 * @note        Detection principle: exist器件的末address写如0X55, 然后AgainreadPick, ifreadGet the valuefor0X55
 *              It means that the detection is normal. otherwise,It meansDetection failed.
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

    if (temp == 0X55)                       /* readGet data正常 */
    {
        return 0;
    }
    else                                    /* Exclude the first initialization */
    {
        at24cxx_write_one_byte(addr, 0X55); /* Write data first */
        temp = at24cxx_read_one_byte(255);  /* AgainreadGet data */

        if (temp == 0X55)
        {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief       existAT24CXX里面的指定address开始readOutput the specified number of data
 * @param       addr    : The address to start reading right24c02for0~255
 * @param       pbuf    : Data array first address
 * @param       datalen : wantreadNumber of data output
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
 * @param       datalen : Number of data to be written
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
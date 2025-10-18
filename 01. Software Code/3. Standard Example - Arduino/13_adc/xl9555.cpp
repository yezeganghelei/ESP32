/**
 ****************************************************************************************************
 * @file        xl9555.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       xl9555 driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#include "xl9555.h"
#include <Wire.h>

/**
* @brief       initializationIOexpansion chip
* @param       none
* @retval      none
*/
void xl9555_init(void)
{
    pinMode(IIC_INT_PIN, INPUT_PULLUP);     /* Configure the interrupt pin */

    Wire.begin(IIC_SDA, IIC_SCL, 400000);   /* Initialize IIC connection */

    /* Power onFirstreadClear interrupt flag once */
    xl9555_read_port(0);
    xl9555_read_port(1);
}

/**
 * @brief Write data to XL9555 related registers
 * @param reg: register address
 * @param data: data written to the register
 * @retval None
 */
void xl9555_write_reg(uint8_t reg, uint8_t data)
{
    Wire.beginTransmission(EXIO_ADDR);        /* sendSlave machineof7Bit device address to send queue */
    Wire.write(reg);                          /* Send the address to be written to the slave register to the send queue */
    Wire.write(data);                         /* Send data to be written to the slave register to the send queue */
    Wire.endTransmission();                   /* IIC send Send the queue data(Withoutparameter,expresssendstopSignal,end transfer) */          
}

/**
 * @brief       TowardsXL9555Read data from relevant registers
 * @param       reg    : Register address
 * @retval      Register value / 0xFF:No data received
 */
uint8_t xl9555_read_reg(uint8_t reg)
{
    Wire.beginTransmission(EXIO_ADDR);        /* sendSlave machineof7Bit device address to send queue */
    Wire.write(reg);                          /* sendwantreadSlave machineofRegister addressarrivesendqueue */
    Wire.endTransmission(0);                  /* IIC send Send the queue data(Pass parameters as0,Indicates resending astartSignal,KeepIICbus validconnect) */

    Wire.requestFrom(EXIO_ADDR, 1);           /* HostTowardsSlave machinesenddata request,And get the data */
    if (Wire.available() != 0)                /* Get the number of data bytes that have been received */
    {
        return Wire.read();                   /* Read data to the data buffer */
    }

    return 0xFF;
}

/**
 * @brief       set upXL9555ofP0orP1portoutput status
 * @param       portx : P0 / P1
 * @param       data  : IOstatus(correspond8indivualIO)
 * @retval      none
 */
void xl9555_write_port(uint8_t portx, uint8_t data)
{
    xl9555_write_reg(portx ? XL9555_OUTPUT_PORT1_REG : XL9555_OUTPUT_PORT0_REG, data);
}

/**
 * @brief       readXL9555ofP0orP1portstatus
 * @param       portx : P0 / P1
 * @retval      IOstatus(correspond8indivualIO)
 */
uint8_t xl9555_read_port(uint8_t portx)
{
    return xl9555_read_reg(portx ? XL9555_INPUT_PORT1_REG : XL9555_INPUT_PORT0_REG);
}

/**
 * @brief set upXL9555SomeIOmode(Output or input)
 * @param port_pin: To set upofIOserial number,P0~7orP1~7
 * @param mode: IO_SET_OUTPUT / IO_SET_INPUT
 * @retval none
 */
void xl9555_io_config(uint16_t port_pin, io_mode_t mode)
{
    uint8_t config_reg = 0;
    uint8_t config_value = 0;

    config_reg  = xl9555_read_reg(port_pin > XL_PORT0_ALL_PIN ? XL9555_CONFIG_PORT1_REG : XL9555_CONFIG_PORT0_REG);   /* Firstreadset upPinWhereofRegister status */

    if (mode == IO_SET_OUTPUT)    /* according to modeparameter set upInput and output conditions，cannot affect otherIO */
    {
        config_value = config_reg & (~(port_pin >> (port_pin > XL_PORT0_ALL_PIN ? 8 : 0)));   /* getSomeIOset upAfter the output functionofPORTvalue but does not affect theset upofotherIOstatus */
    }
    else
    {
        config_value = config_reg | (port_pin >> (port_pin > XL_PORT0_ALL_PIN ? 8 : 0));      /* getSomeIOset as input functionPORTvalue but does not affect theset upofotherIOstatus */
    }

    xl9555_write_reg(port_pin > XL_PORT0_ALL_PIN ? XL9555_CONFIG_PORT1_REG : XL9555_CONFIG_PORT0_REG, config_value);    /* Set IO input and output status to the configuration register */
}

/**
 * @brief       set upXL9555Configured as output functionIOoutput status(High levelorLow level)
 * @param       port_pin  : alreadyset upGood output functionofIOserial number
 * @param       state      : IO_SET_LOW / IO_SET_HIGH
 * @retval      none
 */
void xl9555_pin_set(uint16_t port_pin, io_state_t state)
{
    uint8_t pin_reg = 0;
    uint8_t pin_value = 0;

    pin_reg = xl9555_read_reg(port_pin > XL_PORT0_ALL_PIN ? XL9555_OUTPUT_PORT1_REG : XL9555_OUTPUT_PORT0_REG);     /* Firstreadset upPinWhereofRegister status */

    if (state == IO_SET_HIGH)    /* according to stateparameter set upIOofhighLow level */
    {
        pin_value = pin_reg | (port_pin >> (port_pin > XL_PORT0_ALL_PIN ? 8 : 0));          /* getSomeIOAfter setting to high levelPORTvalue but does not affect theset upofotherIOstatus */
    }
    else
    {
        pin_value = pin_reg & (~(port_pin >> (port_pin > XL_PORT0_ALL_PIN ? 8 : 0)));       /* getSomeIOset upforLow levelbackofPORTvalue but does not affect theset upofotherIOstatus */
    }

    xl9555_write_reg(port_pin > XL_PORT0_ALL_PIN ? XL9555_OUTPUT_PORT1_REG : XL9555_OUTPUT_PORT0_REG, pin_value);   /* Set IO high and low level status to the output register */
}

/**
 * @brief       GetXL9555Configured as input functionIOstatus(High levelorLow level)
 * @param       port_pin  : alreadyset upGood input functionofIOserial number
 * @retval      0Low level / 1High level
 */
uint8_t xl9555_get_pin(uint16_t port_pin)
{
    uint8_t pin_state = 0;
    uint8_t port_value = 0;

    port_value = xl9555_read_reg(port_pin > XL_PORT0_ALL_PIN ? XL9555_INPUT_PORT1_REG : XL9555_INPUT_PORT0_REG);  /* Read the status of the port where the pin is located: 1 is not pressed, 0 is pressed */
    pin_state = port_pin >> (port_pin > XL_PORT0_ALL_PIN ? 8 : 0);    /* if yesPORT1ofPINNeed to move right first8Bit */
    pin_state = pin_state & port_value;                               /* getneedwantQueryBitstatus */

    return pin_state ? 1 : 0;
}
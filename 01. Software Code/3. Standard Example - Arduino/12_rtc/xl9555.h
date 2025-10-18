/**
 ******************************************************************************************************
 * @file xl9555.h
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief xl9555 driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ******************************************************************************************************
 */

#ifndef __XL9555_H
#define __XL9555_H

#include "Arduino.h"

/* pindefinition */
#define IIC_SCL       42
#define IIC_SDA       41
#define IIC_INT_PIN   0     /* Need to connect with jumper cap */

#define EXIO_ADDR     0x20  /* 7-bit device address */

#define IIC_INT       digitalRead(IIC_INT_PIN) 

/* IOExpand chipXL9555ofeachIOFunction */
#define KEY0                          XL_PIN_P17  /* button0pin P17 */    
#define KEY1                          XL_PIN_P16  /* button1pin P16 */    
#define KEY2                          XL_PIN_P15  /* Button 2 pin P15 */    
#define KEY3                          XL_PIN_P14  /* button3pin P14 */    
#define SLCD_PWR                      XL_PIN_P13  /* SPI_LCD control backlight pin P13 */    
#define SLCD_RST                      XL_PIN_P12  /* SPI_LCDreset pin P12 */    
#define CT_RST                        XL_PIN_P11  /* Touch screen interrupt pin P11 */    
#define LCD_BL                        XL_PIN_P10  /* RGB screen backlight control pin P10 */    
#define GBC_KEY                       XL_PIN_P07  /* ATK_MODULEinterfaceKEYpin P07 */
#define GBC_LED                       XL_PIN_P06  /* ATK_MODULEinterfaceLEDpin P06 */
#define OV_RESET                      XL_PIN_P05  /* Camerareset pin P05 */
#define OV_PWDN                       XL_PIN_P04  /* Camera standbypin P04 */
#define BEEP                          XL_PIN_P03  /* Buzzer control pin P03 */
#define SPK_EN                        XL_PIN_P02  /* Amplitude enable pin P02 */
#define QMA_INT                       XL_PIN_P01  /* QMA6100P interrupt pin P01 */
#define AP_INT                        XL_PIN_P00  /* AP3216C interrupt pin P00 */

/* device register */
#define XL9555_INPUT_PORT0_REG        0           /* Input Register：for readingP0Input value of the port */
#define XL9555_INPUT_PORT1_REG        1           /* Input Register：for readingP1Input value of the port */
#define XL9555_OUTPUT_PORT0_REG       2           /* Output Register ：used forset upP0portofOutput value */
#define XL9555_OUTPUT_PORT1_REG       3           /* Output Register: used to set upP1 port of output value */
#define XL9555_INVERSION_PORT0_REG    4           /* polarity inversion register：Used whenP0When the port is used as input，Invert the input level，That is, when the pin is high，set upThis register corresponds toofThe bit is1hour，readarriveofInput Register0，1The value is low0 */
#define XL9555_INVERSION_PORT1_REG    5           /* polarity inversion register：Used whenP1When the port is used as input，Invert the input level，That is, when the pin is high，set upThis register corresponds toofThe bit is1hour，readarriveofInput Register0，1The value is low0 */
#define XL9555_CONFIG_PORT0_REG       6           /* configuration register：For configurationP0port as input(1)or output(0) */
#define XL9555_CONFIG_PORT1_REG       7           /* configuration register：For configurationP1port as input(1)or output(0) */

/* XL9555eachIOFunctions */
#define XL_PIN_P00       				      0x0001
#define XL_PIN_P01       				      0x0002
#define XL_PIN_P02       				      0x0004
#define XL_PIN_P03       				      0x0008
#define XL_PIN_P04       				      0x0010
#define XL_PIN_P05       				      0x0020
#define XL_PIN_P06       				      0x0040
#define XL_PIN_P07       				      0x0080
#define XL_PIN_P10      				      0x0100
#define XL_PIN_P11      				      0x0200
#define XL_PIN_P12      				      0x0400
#define XL_PIN_P13      				      0x0800
#define XL_PIN_P14      				      0x1000
#define XL_PIN_P15      				      0x2000
#define XL_PIN_P16      				      0x4000
#define XL_PIN_P17      				      0x8000

#define XL_PORT0_ALL_PIN              0x00FF
#define XL_PORT1_ALL_PIN              0xFF00

/* IO configuration mode */
typedef enum
{
  IO_SET_OUTPUT = 0x00,  
  IO_SET_INPUT,       
} io_mode_t;

/* IOConfigure output high and low levels */
typedef enum
{
  IO_SET_LOW = 0x00,  
  IO_SET_HIGH,       
} io_state_t;

/* function declaration */
void xl9555_init(void);                                     /* initializationIOExpand chip */
void xl9555_write_reg(uint8_t reg, uint8_t data);           /* Write data to XL9555 related registers */
uint8_t xl9555_read_reg(uint8_t reg);                       /* Read data to XL9555 related registers */
void xl9555_write_port(uint8_t portx, uint8_t data);        /* set upXL9555ofP0orP1portoutput status */
uint8_t xl9555_read_port(uint8_t portx);                    /* readXL9555ofP0orP1 port status */
void xl9555_io_config(uint16_t port_pin, io_mode_t mode);   /* set upXL9555SomeIOmode(Output or input) */
void xl9555_pin_set(uint16_t port_pin, io_state_t state);   /* Set the output status (high level or low level) of the IO configured as output function by XL9555 */
uint8_t xl9555_get_pin(uint16_t port_pin);                  /* getXL9555Configured as input functionIOstatus(high level or low level) */

#endif
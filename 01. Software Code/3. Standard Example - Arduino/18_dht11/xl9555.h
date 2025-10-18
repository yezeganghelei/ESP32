/**
 ******************************************************************************************************
 * @file xl9555.h
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief xl9555 driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************
 *@attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ******************************************************************************************************
 */

#ifndef __XL9555_H
#define __XL9555_H

#include "Arduino.h"

/* Pindefinition */
#define IIC_SCL       42
#define IIC_SDA       41
#define IIC_INT_PIN   0     /* Need to connect with jumper cap */

#define EXIO_ADDR     0x20  /* 7-bit device address */

#define IIC_INT       digitalRead(IIC_INT_PIN) 

/* IOExpand chipXL9555ofeachIOFunction */
#define KEY0                          XL_PIN_P17  /* button0Pin P17 */    
#define KEY1                          XL_PIN_P16  /* button1Pin P16 */    
#define KEY2                          XL_PIN_P15  /* Button 2 pin P15 */    
#define KEY3                          XL_PIN_P14  /* button3Pin P14 */    
#define SLCD_PWR                      XL_PIN_P13  /* SPI_LCD control backlight pin P13 */    
#define SLCD_RST                      XL_PIN_P12  /* SPI_LCDreset pin P12 */    
#define CT_RST                        XL_PIN_P11  /* Touch screen interrupt pin P11 */    
#define LCD_BL                        XL_PIN_P10  /* RGB screen backlight control pin P10 */    
#define GBC_KEY                       XL_PIN_P07  /* ATK_MODULEinterfaceKEYPin P07 */
#define GBC_LED                       XL_PIN_P06  /* ATK_MODULEinterfaceLEDPin P06 */
#define OV_RESET                      XL_PIN_P05  /* Camerareset pin P05 */
#define OV_PWDN                       XL_PIN_P04  /* CamerastandbyPin P04 */
#define BEEP                          XL_PIN_P03  /* Buzzer control pin P03 */
#define SPK_EN                        XL_PIN_P02  /* Amplitude enable pin P02 */
#define QMA_INT                       XL_PIN_P01  /* QMA6100P Interrupt Pin P01 */
#define AP_INT                        XL_PIN_P00  /* AP3216C Interrupt Pin P00 */

/* Device Register */
#define XL9555_INPUT_PORT0_REG        0           /* Input Register：Used for readingP0Input value of the port */
#define XL9555_INPUT_PORT1_REG        1           /* Input Register：Used for readingP1Input value of the port */
#define XL9555_OUTPUT_PORT0_REG       2           /* Output register ：used forset upP0portofOutput value */
#define XL9555_OUTPUT_PORT1_REG       3           /* Output register ：used forset upP1portofOutput value */
#define XL9555_INVERSION_PORT0_REG    4           /* polarity inversion register：used whenP0When the port is used as input，Reversal of the input level，That is, when the pin is high level，set upThis register corresponds toofThe bit is1hour，readarriveofInput Register0，1The value is low0 */
#define XL9555_INVERSION_PORT1_REG    5           /* polarity inversion register：used whenP1When the port is used as input，Reversal of the input level，That is, when the pin is high level，set upThis register corresponds toofThe bit is1hour，readarriveofInput Register0，1The value is low0 */
#define XL9555_CONFIG_PORT0_REG       6           /* Configure registers：For configurationP0Port as input(1)Or output(0) */
#define XL9555_CONFIG_PORT1_REG       7           /* Configure registers：For configurationP1Port as input(1)Or output(0) */

/* XL9555eachIOfunction */
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
uint8_t xl9555_read_reg(uint8_t reg);                       /* Read data from XL9555 related registers */
void xl9555_write_port(uint8_t portx, uint8_t data);        /* set upXL9555ofP0orP1portoutput status */
uint8_t xl9555_read_port(uint8_t portx);                    /* readXL9555ofP0orP1portstatus */
void xl9555_io_config(uint16_t port_pin, io_mode_t mode);   /* set upXL9555someoneIOmode(output or input) */
void xl9555_pin_set(uint16_t port_pin, io_state_t state);   /* Set the output status of the IO configured as the output function of XL9555 (high or low) */
uint8_t xl9555_get_pin(uint16_t port_pin);                  /* GetXL9555Configured as input functionIOstatus(high level or low level) */

#endif
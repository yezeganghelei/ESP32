/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file GT9XXX.h
 * @author team()
 * @version V1.1
 * @date 2023-12-1
 * @brief 4.3-inch capacitor touchscreen-GT9xxx drive code
 * @note GT series capacitor touchscreenIC universal drive, This code supports: GT9147/GT917S/GT968/GT1151/GT9271 Many more
 * driveIC, these driveIConlyIDno the same, the specific code basically requires no modifications to be directly driven through this code

 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#ifndef __GT9XXX_H
#define __GT9XXX_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "touch.h"
#include "string.h"
#include "iic.h"
#include "xl9555.h"

/******************************************************************************************/
/* GT9XXX INTPin definition */
#define GT9XXX_INT_GPIO_PIN             GPIO_NUM_40
#define GT9XXX_IIC_PORT                 I2C_NUM_1
#define GT9XXX_IIC_SDA                  GPIO_NUM_39
#define GT9XXX_IIC_CLK                  GPIO_NUM_38
#define GT9XXX_IIC_FREQ                 400000                                  /* IIC FREQ */
#define GT9XXX_INT                      gpio_get_level(GT9XXX_INT_GPIO_PIN)     /* InterruptPin */

/* RGB_BL */
#define CT_RST(x)       do { x ?                              \
                            xl9555_pin_write(CT_RST_IO, 1):   \
                            xl9555_pin_write(CT_RST_IO, 0);   \
                        } while(0)

/* IICRead and write commands */
#define GT9XXX_CMD_WR                   0X28        /* Write a command */
#define GT9XXX_CMD_RD                   0X29        /* Read command */

/* GT9XXX partregisterdefinition  */
#define GT9XXX_CTRL_REG                 0X8040      /* GT9XXX Control Register */
#define GT9XXX_CFGS_REG                 0X8047      /* GT9XXXConfigure the start address register */
#define GT9XXX_CHECK_REG                0X80FF      /* GT9XXX Checksum Register */
#define GT9XXX_PID_REG                  0X8140      /* GT9XXXproductIDregister */

#define GT9XXX_GSTID_REG                0X814E      /* GT9XXX currently detected touch situation */
#define GT9XXX_TP1_REG                  0X8150      /* The first touch point data address */
#define GT9XXX_TP2_REG                  0X8158      /* The second touch point data address */
#define GT9XXX_TP3_REG                  0X8160      /* The third touch point data address */
#define GT9XXX_TP4_REG                  0X8168      /* The fourth touch point data address */
#define GT9XXX_TP5_REG                  0X8170      /* The fifth touch point data address */
#define GT9XXX_TP6_REG                  0X8178      /* The sixth touch point data address */
#define GT9XXX_TP7_REG                  0X8180      /* The seventh touch point data address */
#define GT9XXX_TP8_REG                  0X8188      /* The eighth touch point data address */
#define GT9XXX_TP9_REG                  0X8190      /* The ninth touch point data address */
#define GT9XXX_TP10_REG                 0X8198      /* The tenth touch point data address */

/* Function declaration */
uint8_t gt9xxx_init(void);                                          /* initializationgt9xxxtouchscreen */
uint8_t gt9xxx_scan(uint8_t mode);                                  /* scanningtouchscreen(Using query method) */
esp_err_t gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len);   /* Write data to gt9xxx */
esp_err_t gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len);   /* fromgt9xxxRead out the data */

#endif
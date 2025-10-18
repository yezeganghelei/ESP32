/**
 ******************************************************************************************************
 * @file ap3216c.h
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief AP3216C driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************
 * @attention

 ******************************************************************************************************
 */
 
#ifndef __AP3216C_H__
#define __AP3216C_H__

#include "xl9555.h"
#include "driver/gpio.h"

/* AP3216Cinformation */
typedef struct _ap3216c_value_t
{
    uint16_t ir;
    uint16_t als;
    uint16_t ps;
} ap3216c_value_t;

/* Related parameter definitions */
#define AP3216C_INT     xl9555_pin_read(AP_INT_IO)
#define AP3216C_ADDR    0X1E                                        /* AP3216Caddress */

/* function declaration */
void ap3216c_init(i2c_obj_t self);                                  /* Initialize AP3216C */
uint8_t ap3216c_comfig(void);                                       /* examineAP3216C */
void ap3216c_read_data(uint16_t *ir, uint16_t *ps, uint16_t *als);  /* readAP3216Cdata */

#endif
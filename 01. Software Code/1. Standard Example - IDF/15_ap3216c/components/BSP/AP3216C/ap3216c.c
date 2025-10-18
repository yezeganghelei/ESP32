/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file ap3216c.c
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief AP3216C driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#define __IIC_VERSION__  "1.0"

#include "ap3216c.h"

i2c_obj_t ap3216c_master;

/**
 * @brief       Readap3216cof16BitIOvalue
 * @param       data:Storage area
 * @param       reg :register
 * @retval      ESP_OK:Readsuccess；other:Read failed
 */
static esp_err_t ap3216c_read_one_byte(uint8_t* data, uint8_t reg)
{
    uint8_t reg_buf[1];

    reg_buf[0] = reg;

    i2c_buf_t buf[2] = {
        {.len = 1, .buf = reg_buf},
        {.len = 1, .buf = data},
    };

    i2c_transfer(&ap3216c_master, AP3216C_ADDR, 2, buf, I2C_FLAG_WRITE | I2C_FLAG_READ | I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief       Towardsap3216cWrite16BitIOvalue
 * @param       data:wantWriteData of
 * @retval      ESP_OK:Readsuccess；other:Read failed
 */
static esp_err_t ap3216c_write_one_byte(uint8_t reg, uint8_t data)
{
    uint8_t reg_buf[1];

    reg_buf[0] = reg;
    
    i2c_buf_t buf[2] = {
        {.len = 1, .buf = reg_buf},
        {.len = 1, .buf = &data},
    };

    i2c_transfer(&ap3216c_master, AP3216C_ADDR, 2, buf, I2C_FLAG_STOP); /* Transmit sensor address */
    return ESP_OK;
}

/**
 * @brief       initializationAP3216C
 * @param       none
 * @retval      0, success;
                1, fail;
*/
uint8_t ap3216c_comfig(void)
{
    uint8_t temp;
    
    ap3216c_write_one_byte(0x00, 0X04); /* complexBitAP3216C */
    vTaskDelay(50);                     /* AP33216CcomplexBitAt least10ms */
    ap3216c_write_one_byte(0x00, 0X03); /* OpenALS、PS+IR */
    ap3216c_read_one_byte(&temp,0X00);  /* Read the just written0X03 */

    if (temp == 0X03)
    {
        ESP_LOGE("ap3216c", "AP3216C success!!!");
        return 0;                       /* AP3216C is normal */
    }
    else
    {
        ESP_LOGE("ap3216c", "AP3216C fail!!!");
        return 1;                       /* AP3216Cfail */
    }
} 

/**
 * @brief       ReadAP3216CData of
 * @note        Read raw data，includeALS,PSandIR
 *              If open at the same timeALS,IR+PSThe interval between the two data readings should be greater than112.5ms
 * @param       ir  :IRsensorvaluepointer
 * @param       ps  :PSsensorvaluepointer
 * @param       als :ALSsensorvaluepointer
 * @retval      none
 */
void ap3216c_read_data(uint16_t *ir, uint16_t *ps, uint16_t *als)
{
    uint8_t buf[6] = {0};
    uint8_t i;

    for (i = 0; i < 6; i++)
    {
        ap3216c_read_one_byte(&buf[i],0X0A + i);                    /* Read all sensor data in a loop */
    }

    if (buf[0] & 0X80)
    {
        *ir = 0;                                                    /* IR_OFBitfor1,Then datanoneeffect */
    }
    else 
    {
        *ir = ((uint16_t)buf[1] << 2) | (buf[0] & 0X03);            /* ReadIRSensor data   */
    }

    *als = ((uint16_t)buf[3] << 8) | buf[2];                        /* ReadALSSensor data   */ 

    if (buf[4] & 0x40) 
    {
        *ps = 0;                                                    /* IR_OFBitfor1,Then datanoneeffect */
    }
    else
    {
        *ps = ((uint16_t)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F);   /* ReadPSSensor data */
    }
}

/**
 * @brief       initializationAP3216C
 * @param       none
 * @retval      none
 */
void ap3216c_init(i2c_obj_t self)
{
    if (self.init_flag == ESP_FAIL)
    {
        iic_init(I2C_NUM_0);    /* initializationIIC */
    }

    ap3216c_master = self;

    while (ap3216c_comfig())    /* AP3216C not detected */
    {
        ESP_LOGE("ap3216c", "ap3216c init fail!!!");
        vTaskDelay(500);
    }
}
/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
* @file xl9555.c
* @author
* @version V1.0
* @date 2023-08-26
* @brief XL9555 driver code
* @license Copyright (c) 2020-2032,
******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
* @attention

******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
*/

#include "xl9555.h"

i2c_obj_t xl9555_i2c_master;
static uint16_t xl9555_failed = 0;

/**
 * @brief       ReadXL9555of16BitIOvalue
 * @param       data：ReaddataofStorage area
 * @param       len：Readdataofsize
 * @retval      ESP_OK：Readsuccess；other：Read failed
 */
esp_err_t xl9555_read_byte(uint8_t *data, size_t len)
{
    uint8_t memaddr_buf[1];
    memaddr_buf[0]  = XL9555_INPUT_PORT0_REG;

    i2c_buf_t bufs[2] = {
        {.len = 1, .buf = memaddr_buf},
        {.len = len, .buf = data},
    };

    return i2c_transfer(&xl9555_i2c_master, XL9555_ADDR, 2, bufs, I2C_FLAG_WRITE | I2C_FLAG_READ | I2C_FLAG_STOP);
}

/**
 * @brief       TowardsXL9555Write16BitIOvalue
 * @param       reg：Register address
 * @param       data：wantWriteofdata
 * @param       len：The size of the data to be written
 * @retval      ESP_OK：Readsuccess；other：Read failed
 */
esp_err_t xl9555_write_byte(uint8_t reg, uint8_t *data, size_t len)
{
    i2c_buf_t bufs[2] = {
        {.len = 1, .buf = &reg},
        {.len = len, .buf = data},
    };

    return i2c_transfer(&xl9555_i2c_master, XL9555_ADDR, 2, bufs, I2C_FLAG_STOP);
}

/**
 * @brief       Control a certainIOofLevel
 * @param       pin     : ControlledIO
 * @param       val     : Level
 * @retval      Return to allIOstate
 */
uint16_t xl9555_pin_write(uint16_t pin, int val)
{
    uint8_t w_data[2];
    uint16_t temp = 0x0000;

    xl9555_read_byte(w_data, 2);

    if (pin <= GBC_KEY_IO)
    {
        if (val)
        {
            w_data[0] |= (uint8_t)(0xFF & pin);
        }
        else
        {
            w_data[0] &= ~(uint8_t)(0xFF & pin);
        }
    }
    else
    {
        if (val)
        {
            w_data[1] |= (uint8_t)(0xFF & (pin >> 8));
        }
        else
        {
            w_data[1] &= ~(uint8_t)(0xFF & (pin >> 8));
        }
    }

    temp = ((uint16_t)w_data[1] << 8) | w_data[0]; 

    xl9555_write_byte(XL9555_OUTPUT_PORT0_REG, w_data, 2);

    return temp;
}

/**
 * @brief       Get a certainIOstate
 * @param       pin     : want获取stateofIO
 * @retval      thisIOmouthofvalue(state, 0/1)
 */
int xl9555_pin_read(uint16_t pin)
{
    uint16_t ret;
    uint8_t r_data[2];

    xl9555_read_byte(r_data, 2);

    ret = r_data[1] << 8 | r_data[0];

    return (ret & pin) ? 1 : 0;
}

/**
 * @brief       XL9555ofIOConfiguration
 * @param       config_value：IOConfigurationInput or output
 * @retval      Returns the set value
 */
uint16_t xl9555_ioconfig(uint16_t config_value)
{
    /* Slave address + CMD + data1(P0) + data2(P1) */
    /* P00、P01、P14、P15、P16、P17For input，The other pins are outputs -->1111 0000 0000 0011 Notice：0For output，1For input*/
    uint8_t data[2];
    esp_err_t err;
    int retry = 3;

    data[0] = (uint8_t)(0xFF & config_value);
    data[1] = (uint8_t)(0xFF & (config_value >> 8));

    do
    {
        err = xl9555_write_byte(XL9555_CONFIG_PORT0_REG, data, 2);
        
        if (err != ESP_OK)
        {
            retry--;
            vTaskDelay(100); 
            ESP_LOGE("IIC", "%s configure %X failed, ret: %d", __func__, config_value, err);
            xl9555_failed = 1;
            
            if ((retry <= 0) && xl9555_failed)
            {
                vTaskDelay(5000); 
                esp_restart();
            }
        }
        else
        {
            xl9555_failed = 0;
            break;
        }
        
    } while (retry);
    
    return config_value;
}

/**
 * @brief       initializationXL9555
 * @param       none
 * @retval      none
 */
void xl9555_init(i2c_obj_t self)
{
    uint8_t r_data[2];

    if (self.init_flag == ESP_FAIL)
    {
        iic_init(I2C_NUM_0);        /* initializationIIC */
    }

    xl9555_i2c_master = self;
    gpio_config_t gpio_init_struct = {0};
    
    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;
    gpio_init_struct.mode = GPIO_MODE_INPUT;
    gpio_init_struct.pin_bit_mask = (1ull << XL9555_INT_IO);
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&gpio_init_struct);     /* Configure the XL_INT pin */

    /* Read the clear interrupt flag once on power */
    xl9555_read_byte(r_data, 2);
    
    xl9555_ioconfig(0xF003);
    xl9555_pin_write(BEEP_IO, 1);
    xl9555_pin_write(SPK_EN_IO, 1);
}

/**
 * @brief       Key scan function
 * @param       mode:0->Discontinuous;1->continuous
 * @retval      keyvalue, Definition is as follows:
 *              KEY0_PRES, 1, KEY0press
 *              KEY1_PRES, 2, KEY1press
 *              KEY2_PRES, 3, KEY2press
 *              KEY3_PRES, 4, KEY3press
 */
uint8_t xl9555_key_scan(uint8_t mode)
{
    uint8_t keyval = 0;
    static uint8_t key_up = 1;                                          /* Press the button to release the sign */

    if (mode)
    {
        key_up = 1;                                                     /* Supports continuous press */
    }
    
    if (key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0  || KEY3 == 0 )) /* The key release sign is1, And there is any keypressGot it */
    {
        vTaskDelay(10);                                                 /* De-shake */
        key_up = 0;

        if (KEY0 == 0)
        {
            keyval = KEY0_PRES;
        }

        if (KEY1 == 0)
        {
            keyval = KEY1_PRES;
        }

        if (KEY2 == 0)
        {
            keyval = KEY2_PRES;
        }

        if (KEY3 == 0)
        {
            keyval = KEY3_PRES;
        }
    }
    else if (KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && KEY3 == 1)          /* 没有任何按keypress, Mark button released */
    {
        key_up = 1;
    }

    return keyval;                                                      /* Return key value */
}
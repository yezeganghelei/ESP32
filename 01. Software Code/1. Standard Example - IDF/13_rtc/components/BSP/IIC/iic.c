/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file iic.c
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief IIC driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "iic.h"

i2c_obj_t iic_master[I2C_NUM_MAX];  /* forIIC0andIIC1Definitions separatelyIICControl blockStructure */

/**
 * @brief Initialize IIC
 * @param iic_port: I2C number (I2C_NUM_0 / I2C_NUM_1)
 * @retval IIC control block 0 / IIC control block 1
 */
i2c_obj_t iic_init(uint8_t iic_port)
{
    uint8_t i;
    i2c_config_t iic_config_struct = {0};

    if (iic_port == I2C_NUM_0)
    {
        i = 0;
    }
    else
    {
        i = 1;
    }
    
    iic_master[i].port = iic_port;
    iic_master[i].init_flag = ESP_FAIL;

    if (iic_master[i].port == I2C_NUM_0)
    {
        iic_master[i].scl = IIC0_SCL_GPIO_PIN;
        iic_master[i].sda = IIC0_SDA_GPIO_PIN;
    }
    else
    {
        iic_master[i].scl = IIC1_SCL_GPIO_PIN;
        iic_master[i].sda = IIC1_SDA_GPIO_PIN;
    }

    iic_config_struct.mode = I2C_MODE_MASTER;                               /* Set IIC mode-host mode */
    iic_config_struct.sda_io_num = iic_master[i].sda;                       /* set upIIC_SDAPin */
    iic_config_struct.scl_io_num = iic_master[i].scl;                       /* Set the IIC_SCL pin */
    iic_config_struct.sda_pullup_en = GPIO_PULLUP_ENABLE;                   /* ConfigurationIIC_SDAPin pull-up enable */
    iic_config_struct.scl_pullup_en = GPIO_PULLUP_ENABLE;                   /* ConfigurationIIC_SCLPin pull-up enable */
    iic_config_struct.master.clk_speed = IIC_FREQ;                          /* set upIICCommunication rate */
    i2c_param_config(iic_master[i].port, &iic_config_struct);               /* set upIICInitialization parameters */

    /* activationI2CDriver of controller */
    iic_master[i].init_flag = i2c_driver_install(iic_master[i].port,        /* Port number */
                                                 iic_config_struct.mode,    /* host mode */
                                                 I2C_MASTER_RX_BUF_DISABLE, /* Receive cache size in slave mode(Host mode is not used) */
                                                 I2C_MASTER_TX_BUF_DISABLE, /* From the machinemodelSend cache size(Host mode is not used) */     
                                                 0);                        /* Flag used to allocate interrupts(Usually used in slave mode) */            

    if (iic_master[i].init_flag != ESP_OK)
    {
        while(1)
        {
            printf("%s , ret: %d", __func__, iic_master[i].init_flag);
            vTaskDelay(1000);
        }
    }

    return iic_master[i];
}

/**
 * @brief       IICRead and write data
 * @param       self：Equipment control block
 * @param       addr：Device address
 * @param       n   ：Data size
 * @param       bufs：The data to be sent orreadThe storage area to be picked
 * @param       flags：readwrite flag
 * @retval      none
 */
esp_err_t i2c_transfer(i2c_obj_t *self, uint16_t addr, size_t n, i2c_buf_t *bufs, unsigned int flags)
{
    int data_len = 0;
    esp_err_t ret = ESP_FAIL;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();                                                       /* Create a command link to fill in the command links with a series of data to be sent to the slave. */

    /* Decide based on device communication timingflagsparameter,Then select the following code for different execution situations */
    if (flags & I2C_FLAG_WRITE)
    {
        i2c_master_start(cmd);                                                                          /* Start bit */
        i2c_master_write_byte(cmd, addr << 1, ACK_CHECK_EN);                                            /* slave address + Write operation bit */
        i2c_master_write(cmd, bufs->buf, bufs->len, ACK_CHECK_EN);                                      /* lenData */
        data_len += bufs->len; 
        --n;
        ++bufs;
    }

    i2c_master_start(cmd);                                                                              /* Start bit */
    i2c_master_write_byte(cmd, addr << 1 | (flags & I2C_FLAG_READ), ACK_CHECK_EN);                      /* slave address + read/Write operation bit */

    for (; n--; ++bufs)
    {
        if (flags & I2C_FLAG_READ)
        {
            i2c_master_read(cmd, bufs->buf, bufs->len, n == 0 ? I2C_MASTER_LAST_NACK : I2C_MASTER_ACK); /* readGet data */
        }
        else
        {
            if (bufs->len != 0)
            {
                i2c_master_write(cmd, bufs->buf, bufs->len, ACK_CHECK_EN);                              /* lenData */
            }
        }
        data_len += bufs->len;
    }

    if (flags & I2C_FLAG_STOP)
    {
        i2c_master_stop(cmd);                                                                           /* Stop bit */
    }

    ret = i2c_master_cmd_begin(self->port, cmd, 100 * (1 + data_len) / portTICK_PERIOD_MS);             /* Trigger the I2C controller to execute the command link, that is, the command sending */
    i2c_cmd_link_delete(cmd);                                                                           /* Release the resource used by the command link */

    return ret;
}
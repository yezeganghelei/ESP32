/**
 ****************************************************************************************************
* @file        gt9xxx.c
* @author      team()
* @version     V1.1
* @date        2023-12-1
* @brief       4.3Inch capacitortouchscreen-GT9xxx Driver code
*   @note      GTSeries capacitorstouchscreenICGeneraldrive,This codesupport: GT9147/GT917S/GT968/GT1151/GT9271 Many more
*              driveIC, These driversIConlyIDno the same, The specific code basically does not require any modifications to be directly driven through this code.

* @license     Copyright (c) 2020-2032, 
****************************************************************************************************
* @attention

****************************************************************************************************
*/

#include "gt9xxx.h"

/* Notice: Apart fromGT9271support10TouchOutside, Other touch chips only support 5Touch */
uint8_t g_gt_tnum = 5;      /* defaultsupportoftouchscreenPoints(5Touch) */

/**
 * @brief       Towardsgt9xxxWrite data
 * @param       reg : Start register address
 * @param       buf : Data cache area
 * @param       len : Write data length
 * @retval      esp_err_t ：0, success; 1, fail;
 */
esp_err_t gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, GT9XXX_CMD_WR, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg >> 8, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg & 0XFF, ACK_CHECK_EN);
    i2c_master_write(cmd, buf, len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(GT9XXX_IIC_PORT, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief       fromgt9xxxRead out the data
 * @param       reg : Start register address
 * @param       buf : Data cache area
 * @param       len : Read data length
 * @retval      esp_err_t ：0, success; 1, fail;
 */
esp_err_t gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    if (len == 0)
    {
        return ESP_OK;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, GT9XXX_CMD_WR, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg >> 8, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg & 0XFF, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(GT9XXX_IIC_PORT, cmd, 1000); 
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, GT9XXX_CMD_RD, ACK_CHECK_EN);
    if (len > 1)
    {
        i2c_master_read(cmd, buf, len - 1, 0);
    }
    i2c_master_read_byte(cmd, buf + len - 1, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret |= i2c_master_cmd_begin(GT9XXX_IIC_PORT, cmd, 1000);
    i2c_cmd_link_delete(cmd);

    return ret;
}

/**
 * @brief       initializationgt9xxxtouchscreen
 * @param       none
 * @retval      0, Initialization successfully; 1, Initialization failed;
 */
uint8_t gt9xxx_init(void)
{
    uint8_t temp[5];
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;                 /* Disable pin interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT;                        /* Input mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_DISABLE;              /* Disabled pull-up */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;          /* Disabled pull-down */
    gpio_init_struct.pin_bit_mask = (1ull << GT9XXX_INT_GPIO_PIN);  /* set upofPinbitmask */
    gpio_config(&gpio_init_struct);                                 /* ConfigurationGPIO */

    i2c_config_t iic_config_struct = {0};

    iic_config_struct.mode = I2C_MODE_MASTER;                       /* Set IIC mode-host mode */
    iic_config_struct.sda_io_num = GT9XXX_IIC_SDA;                  /* set upIIC_SDAPin */
    iic_config_struct.scl_io_num = GT9XXX_IIC_CLK;                  /* set upIIC_SCLPin */
    iic_config_struct.sda_pullup_en = GPIO_PULLUP_ENABLE;           /* ConfigurationIIC_SDAPin pull-up enable */
    iic_config_struct.scl_pullup_en = GPIO_PULLUP_ENABLE;           /* ConfigurationIIC_SCLPin pull-up enable */
    iic_config_struct.master.clk_speed = GT9XXX_IIC_FREQ;           /* Set the IIC communication frequency */
    i2c_param_config(GT9XXX_IIC_PORT, &iic_config_struct);          /* set upIICInitialization parameters */

    /* activationI2CDriver of controller */
    i2c_driver_install(GT9XXX_IIC_PORT,                             /* Port number */
                       iic_config_struct.mode,                      /* Host Mode */
                       I2C_MASTER_RX_BUF_DISABLE,                   /* frommachinemodelLower receive buffer size(Host ModeNot used) */
                       I2C_MASTER_TX_BUF_DISABLE,                   /* frommachinemodelSend cache size(Host ModeNot used) */ 
                       0);                                          /* Flags used to assign interrupts(generallyfrommachinemodeluse) */ 

    for (int i = 2;i > 0;i--)
    {
        CT_RST(0);
        vTaskDelay(200);
        CT_RST(1);
        vTaskDelay(200);
    }

    gt9xxx_rd_reg(GT9XXX_PID_REG, temp, 4);     /* Read Product ID */
    temp[4] = 0;
    /* Determine whether it is a specific touchscreen */
    if (strcmp((char *)temp, "911") && strcmp((char *)temp, "9147") && strcmp((char *)temp, "1158") && strcmp((char *)temp, "9271"))
    {
        return 1;   /* If nottouchscreenUsedGT911/9147/1158/9271，Initialization failed，Requires hardware viewing and touchICModel and check whether the timing function is correct */
    }
    printf("CTP ID:%s\r\n", temp);                              /* Print ID */
    
    if (strcmp((char *)temp, "9271") == 0)                      /* ID==9271, support10Touch */
    {
        g_gt_tnum = 10;                                         /* support10pointtouchscreen */
    }

    temp[0] = 0X02;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);                    /* Soft reset GT9XXX */
    
    vTaskDelay(10);
    
    temp[0] = 0X00;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);                    /* End reset, Enter the coordinate read state */

    return 0;
}

/* GT9XXX 10A touch point(most) Corresponding register table */
const uint16_t GT9XXX_TPX_TBL[10] =
{
    GT9XXX_TP1_REG, GT9XXX_TP2_REG, GT9XXX_TP3_REG, GT9XXX_TP4_REG, GT9XXX_TP5_REG,
    GT9XXX_TP6_REG, GT9XXX_TP7_REG, GT9XXX_TP8_REG, GT9XXX_TP9_REG, GT9XXX_TP10_REG,
};

/**
 * @brief       scanningtouchscreen(Using query method)
 * @param       mode : Capacitor screen not usedSecond-rateparameter, To be compatible with resistor screen
 * @retval      Current touch screen status
 *   @arg       0, Touchless touch screen; 
 *   @arg       1, Touch screen with touch;
 */
uint8_t gt9xxx_scan(uint8_t mode)
{
    uint8_t buf[4];
    uint8_t i = 0;
    uint8_t res = 0;
    uint16_t temp;
    uint16_t tempsta;
    static uint8_t t = 0;                                                               /* Control query interval,from and reduce CPUOccupancy */
    t++;

    if ((t % 10) == 0 || t < 10)                                                        /* When I was free,Every entry10Second-rateCTP_ScanFunction detection1Second-rate,from and save CPUUsage rate */
    {
        gt9xxx_rd_reg(GT9XXX_GSTID_REG, &mode, 1);                                      /* Read the state of the touch point */

        if ((mode & 0X80) && ((mode & 0XF) <= g_gt_tnum))
        {
            i = 0;
            gt9xxx_wr_reg(GT9XXX_GSTID_REG, &i, 1);                                     /* Qing sign */
        }

        if ((mode & 0XF) && ((mode & 0XF) <= g_gt_tnum))
        {
            temp = 0XFFFF << (mode & 0XF);                                              /* Convert the number of points to1Number of digits,matchtp_dev.stadefinition */
            tempsta = tp_dev.sta;                                                       /* Save the currenttp_dev.stavalue */
            tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            tp_dev.x[g_gt_tnum - 1] = tp_dev.x[0];                                      /* Save contacts0Data of,Save on the last one */
            tp_dev.y[g_gt_tnum - 1] = tp_dev.y[0];

            for (i = 0; i < g_gt_tnum; i++)
            {
                if (tp_dev.sta & (1 << i))                                              /* Touch effective? */
                {
                    gt9xxx_rd_reg(GT9XXX_TPX_TBL[i], buf, 4);                           /* ReadXYcoordinatevalue */

                    if (tp_dev.touchtype & 0X01)                                        /* Horizontal screen */
                    {
                        tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];
                        tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
                    }
                    else                                                                /* Vertical screen */
                    {
                        tp_dev.x[i] = ltdcdev.width - (((uint16_t)buf[3] << 8) + buf[2]);
                        tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                    }
                }
            }

            res = 1;

            if (tp_dev.x[0] > ltdcdev.width || tp_dev.y[0] > ltdcdev.height)            /* Illegal data(Coordinates exceed) */
            {
                if ((mode & 0XF) > 1)                                                   /* There are other points and data,则复第二个触pointData of到第一个触point. */
                {
                    tp_dev.x[0] = tp_dev.x[1];
                    tp_dev.y[0] = tp_dev.y[1];
                    t = 0;                                                              /* Trigger a Second-rate,Continuous monitoring will be performed at least10Second-rate,This improves hit rate */
                }
                else                                                                    /* Illegal data, this data will be ignored (restored to the original one) */
                {
                    tp_dev.x[0] = tp_dev.x[g_gt_tnum - 1];
                    tp_dev.y[0] = tp_dev.y[g_gt_tnum - 1];
                    mode = 0X80;
                    tp_dev.sta = tempsta;                                               /* Recover tp_dev.sta */
                }
            }
            else 
            {
                t = 0;                                                                  /* Trigger a Second-rate,Continuous monitoring will be performed at least10Second-rate,This improves hit rate */
            }
        }
    }

    if ((mode & 0X8F) == 0X80)                                                          /* Touchless click */
    {
        if (tp_dev.sta & TP_PRES_DOWN)                                                  /* It was pressed before */
        {
            tp_dev.sta &= ~TP_PRES_DOWN;                                                /* Release the mark button */
        }
        else                                                                            /* Haven't been pressed before */
        {
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
            tp_dev.sta &= 0XE000;                                                       /* Clear point valid mark */
        }
    }

    if (t > 240)
    {
        t = 10;                                                                         /* againfrom10Start counting */
    }

    return res;
}
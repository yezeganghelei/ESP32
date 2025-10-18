/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file es8388.c
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief ES8388 driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "es8388.h"

i2c_obj_t es8388_i2c_master;

/**
 * @brief       IICWrite function
 * @param       slave_addr:ES8388address
 * @param       reg_add:Register address
 * @param       data:Write data
 * @retval      none
 */
esp_err_t es8388_write_reg(uint8_t reg_addr, uint8_t data)
{
    i2c_buf_t buf[2] = {
        {.len = 1, .buf = &reg_addr},
        {.len = 1, .buf = &data},
    };

    i2c_transfer(&es8388_i2c_master, ES8388_ADDR >> 1, 2, buf, I2C_FLAG_STOP);

    return ESP_OK;
}

/**
 * @brief read data
 * @param reg_add:Register address
 * @param p_data: read data
 * @retval None
 */
esp_err_t es8388_read_reg(uint8_t reg_addr, uint8_t *pdata)
{
    i2c_buf_t buf[2] = {
        {.len = 1, .buf = &reg_addr},
        {.len = 1, .buf = pdata},
    };

    i2c_transfer(&es8388_i2c_master, ES8388_ADDR >> 1, 2, buf, I2C_FLAG_WRITE | I2C_FLAG_READ | I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief ES8388 Initialization
 * @param None
 * @retval 0, initialization is normal
 * Other, error code
 */
uint8_t es8388_init(i2c_obj_t self)
{
    esp_err_t ret_val = ESP_OK;

    if (self.init_flag == ESP_FAIL)
    {
        iic_init(I2C_NUM_0);        /* Initialize IIC */
    }

    es8388_i2c_master = self;

    ret_val |= es8388_write_reg(0, 0x80);       /* Soft resetES8388 */
    ret_val |= es8388_write_reg(0, 0x00);
    vTaskDelay(100);                            /* Waiting for reset */

    ret_val |= es8388_write_reg(0x01, 0x58);
    ret_val |= es8388_write_reg(0x01, 0x50);
    ret_val |= es8388_write_reg(0x02, 0xF3);
    ret_val |= es8388_write_reg(0x02, 0xF0);

    ret_val |= es8388_write_reg(0x03, 0x09);    /* Microphone bias power off */
    ret_val |= es8388_write_reg(0x00, 0x06);    /* Enablerefer to 500KdriveEnable */
    ret_val |= es8388_write_reg(0x04, 0x00);    /* DACPower Management，No channel opening */
    ret_val |= es8388_write_reg(0x08, 0x00);    /* MCLKNo frequency separation */
    ret_val |= es8388_write_reg(0x2B, 0x80);    /* DACcontrol DACLRCandADCLRCsame */

    ret_val |= es8388_write_reg(0x09, 0x88);    /* ADC L/R PGAGainConfigurationfor+24dB */
    ret_val |= es8388_write_reg(0x0C, 0x4C);    /* The ADC data is selected as left data = left ADC, right data = left ADC The audio data is 16bit */
    ret_val |= es8388_write_reg(0x0D, 0x02);    /* ADCConfiguration MCLK/Sampling rate=256 */
    ret_val |= es8388_write_reg(0x10, 0x00);    /* ADC digital volume control sets signal attenuation L to minimum! ! ! */
    ret_val |= es8388_write_reg(0x11, 0x00);    /* The ADC digital volume control sets the signal attenuation R to minimum! ! ! */

    ret_val |= es8388_write_reg(0x17, 0x18);    /* DAC The audio data is16bit */
    ret_val |= es8388_write_reg(0x18, 0x02);    /* DAC Configuration MCLK/Sampling rate=256 */
    ret_val |= es8388_write_reg(0x1A, 0x00);    /* DACDigital volume control attenuates signal L  Set to minimum！！！ */
    ret_val |= es8388_write_reg(0x1B, 0x00);    /* DAC digital volume control sets signal attenuation R to minimum! ! ! */
    ret_val |= es8388_write_reg(0x27, 0xB8);    /* LMixer */
    ret_val |= es8388_write_reg(0x2A, 0xB8);    /* R mixer */
    vTaskDelay(100);

    if (ret_val != ESP_OK)
    {
        while(1)
        {
            printf("ES8388Initialization failed！！！\r\n");
            vTaskDelay(500);
        }
    }
    else
    {
        printf("ES8388Initialization successfully！！！\r\n");
    }
    
    return 0;
}

/**
 * @brief       ES8388oppositeinitialization
 * @param       none
 * @retval      0,Initialization is normal
 *              other,Error code
 */
esp_err_t es8388_deinit(void)
{
    int res = 0;
    res = es8388_write_reg(0x02, 0xFF);  /* Reset and pause ES8388 */
    return res;
}

/**
 * @brief       set upES8388Working mode
 * @param       fmt : Working mode
 *    @arg      0, Philips StandardI2S;
 *    @arg      1, MSB(Left aligned);
 *    @arg      2, LSB(Right aligned);
 *    @arg      3, PCM/DSP
 * @param       len : Data length
 *    @arg      0, 24bit
 *    @arg      1, 20bit
 *    @arg      2, 18bit
 *    @arg      3, 16bit
 *    @arg      4, 32bit
 * @retval      none
 */
void es8388_sai_cfg(uint8_t fmt, uint8_t len)
{
    fmt &= 0x03;
    len &= 0x07;    /* Limited scope */
    es8388_write_reg(23, (fmt << 1) | (len << 3));  /* R23, ES8388 working mode settings */
}

/**
 * @brief       Set the headphone volume
 * @param       volume : Volume size(0 ~ 33)
 * @retval      none
 */
void es8388_hpvol_set(uint8_t volume)
{
    if (volume > 33)
    {
        volume = 33;
    }

    es8388_write_reg(0x2E, volume);
    es8388_write_reg(0x2F, volume);
}

/**
 * @brief Set the speaker volume
 * @param volume : Volume size (0 ~ 33)
 * @retval None
 */
void es8388_spkvol_set(uint8_t volume)
{
    if (volume > 33)
    {
        volume = 33;
    }

    es8388_write_reg(0x30, volume);
    es8388_write_reg(0x31, volume);
}

/**
 * @brief       set up3DSurround sound
 * @param       depth : 0 ~ 7(3Dstrength,0closure,7Strongest)
 * @retval      none
 */
void es8388_3d_set(uint8_t depth)
{
    depth &= 0x7;       /* Limited scope */
    es8388_write_reg(0x1D, depth << 2);    /* R7, 3D surround settings */
}

/**
 * @brief       ES8388 DAC/ADCConfiguration
 * @param       dacen : dacEnable(1) / closure(0)
 * @param       adcen : adcEnable(1) / closure(0)
 * @retval      none
 */
void es8388_adda_cfg(uint8_t dacen, uint8_t adcen)
{
    uint8_t tempreg = 0;

    tempreg |= !dacen << 0;
    tempreg |= !adcen << 1;
    tempreg |= !dacen << 2;
    tempreg |= !adcen << 3;
    es8388_write_reg(0x02, tempreg);
}

/**
 * @brief       ES8388 DACoutputaisleConfiguration
 * @param       o1en : aisle1Enable(1)/prohibit(0)
 * @param       o2en : aisle2Enable(1)/prohibit(0)
 * @retval      none
 */
void es8388_output_cfg(uint8_t o1en, uint8_t o2en)
{
    uint8_t tempreg = 0;
    tempreg |= o1en * (3 << 4);
    tempreg |= o2en * (3 << 2);
    es8388_write_reg(0x04, tempreg);
}

/**
 * @brief       ES8388 MICGainset up(MIC PGAGain)
 * @param       gain : 0~8, correspond0~24dB  3dB/Step
 * @retval      none
 */
void es8388_mic_gain(uint8_t gain)
{
    gain &= 0x0F;
    gain |= gain << 4;
    es8388_write_reg(0x09, gain);       /* R9,Left and left channelsPGAGainset up */
}

/**
 * @brief       ES8388 ALCset up
 * @param       sel
 *   @arg       0,closureALC
 *   @arg       1,rightaisleALC
 *   @arg       2,Left channelALC
 *   @arg       3,stereoALC
 * @param       maxgain : 0~7,correspond-6.5~+35.5dB
 * @param       minigain: 0~7,correspond-12~+30dB 6dB/STEP
 * @retval      none
 */
void es8388_alc_ctrl(uint8_t sel, uint8_t maxgain, uint8_t mingain)
{
    uint8_t tempreg = 0;
    tempreg = sel << 6;
    tempreg |= (maxgain & 0x07) << 3;
    tempreg |= mingain & 0x07;
    es8388_write_reg(0x12, tempreg);     /* R18,ALCset up */
}

/**
 * @brief ES8388 ADC output channel configuration
 * @param in: input channel
 * @arg 0, channel 1 input
 * @arg 1, channel 2 input
 * @retval None
 */
void es8388_input_cfg(uint8_t in)
{
    es8388_write_reg(0x0A, (5 * in) << 4);   /* ADC1 Input channelchooseL/R	INPUT1 */
}
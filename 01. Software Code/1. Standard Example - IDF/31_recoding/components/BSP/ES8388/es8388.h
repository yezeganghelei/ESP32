/**
 ****************************************************************************************************
 * @file        es8388.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       ES8388Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __ES8388_H__
#define __ES8388_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/i2s.h"
#include "iic.h"
#include "math.h"

#define ES8388_ADDR             0x20                                    /* ES8388Device address,Fixed as0x20 */

/* Declare functions */
uint8_t es8388_init(i2c_obj_t self);                                    /* ES8388 Initialization */
esp_err_t es8388_deinit(void);                                          /* Reset or pauseES8388 Initialization */
esp_err_t es8388_write_reg(uint8_t reg, uint8_t val);                   /* ES8388 Write Register */
esp_err_t es8388_read_reg(uint8_t reg_add, uint8_t *p_data);            /* ES8388 read register */
void es8388_sai_cfg(uint8_t fmt, uint8_t len);                          /* set upSAIWorking mode */
void es8388_hpvol_set(uint8_t volume);                                  /* set upHeadphone volume */
void es8388_spkvol_set(uint8_t volume);                                 /* set up speaker volume */
void es8388_3d_set(uint8_t depth);                                      /* set up3DSurround sound */
void es8388_adda_cfg(uint8_t dacen, uint8_t adcen);                     /* ES8388 DAC/ADCConfiguration */
void es8388_output_cfg(uint8_t o1en, uint8_t o2en);                     /* ES8388 DAC output channel configuration */
void es8388_mic_gain(uint8_t gain);                                     /* ES8388 MIC Gain Settings (MIC PGA Gain) */
void es8388_alc_ctrl(uint8_t sel, uint8_t maxgain, uint8_t mingain);    /* ES8388 ALCset up */
void es8388_input_cfg(uint8_t in);                                      /* ES8388 ADCOutput channelConfiguration */
void sys_vTaskDelay(int ms);
void sys_delay_us(int us);

#endif
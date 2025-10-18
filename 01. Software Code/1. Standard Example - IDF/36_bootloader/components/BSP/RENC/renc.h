/**
 ****************************************************************************************************
 * @file        renc.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       Infrared remote control decoding driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __RENC_H__
#define __RENC_H__

#include <stdint.h>
#include "driver/rmt_encoder.h"

/* Infrared command structure */
typedef struct {
    uint16_t address;
    uint16_t command;
} ir_nec_scan_code_t;

/* Infrared NEC encoder configuration */
typedef struct {
    uint32_t resolution;    /* Encoder resolution，unit：Hz */
} ir_nec_encoder_config_t;

/* Parameter declaration */
esp_err_t rmt_new_ir_nec_encoder(const ir_nec_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder);

#endif
/**
 ****************************************************************************************************
 * @file        remote.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       RMTInfrared decoding driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __REMOTE_H__
#define __REMOTE_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "driver/rmt_rx.h"
#include "ir_nec_encoder.h"
#include "lcd.h"

/* Pin definition */
#define RMT_IN_GPIO_PIN                 GPIO_NUM_2  /* Connect to the GPIO port of REMOTE_IN */
#define RMT_RESOLUTION_HZ               1000000     /* 1MHz frequency, 1 tick = 1us */
#define RMT_NEC_DECODE_MARGIN           200         /* judgeNECTime lengthofTolerancevalue，Less than（value+This value），Greater than（value-This value）For correct */

/* NEC Protocol timing time，Header of agreement9.5ms 4.5ms logic0Two levels of time，logic1Two levels of time，Repeat code two levels of time */
#define NEC_LEADING_CODE_DURATION_0     9000
#define NEC_LEADING_CODE_DURATION_1     4500
#define NEC_PAYLOAD_ZERO_DURATION_0     560
#define NEC_PAYLOAD_ZERO_DURATION_1     560
#define NEC_PAYLOAD_ONE_DURATION_0      560
#define NEC_PAYLOAD_ONE_DURATION_1      1690
#define NEC_REPEAT_CODE_DURATION_0      9000
#define NEC_REPEAT_CODE_DURATION_1      2250

/* Function declaration */
void remote_init(void);                                                                                             /* Initialize RMT */
void remote_scan(rmt_symbol_word_t *rmt_nec_symbols, size_t symbol_num);                                            /* according toNECEncoding and parsing infrared protocol and printing command results */
bool RMT_Rx_Done_Callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data);    /* RMTData reception complete callback function */

#endif
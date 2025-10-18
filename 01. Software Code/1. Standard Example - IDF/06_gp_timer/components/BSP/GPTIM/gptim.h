/**
 ****************************************************************************************************
 * @file        gtim.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       General timer driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __GPTIM_H_
#define __GPTIM_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"

/* Parameter reference */
typedef struct {
    uint64_t event_count;
} gptimer_event_t;

extern QueueHandle_t queue;

/* Function declaration */
void gptim_int_init(uint16_t counts, uint32_t resolution);                                                          /* Initialize the general timer */
bool IRAM_ATTR gptimer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data);  /* Timer callback function */

#endif
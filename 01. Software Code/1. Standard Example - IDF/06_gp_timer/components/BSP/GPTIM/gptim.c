/**
 ****************************************************************************************************
 * @file        gtim.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       General timer driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "gptim.h"

QueueHandle_t queue;

/**
 * @brief       Initialize the general timer
 * @param       counts: Count value
 * @param       resolution: Timer period; resolution = 1s = 1000000us (the timer uses microseconds as its unit)
 * @retval      none
 */
void gptim_int_init(uint16_t counts, uint32_t resolution)
{
    gptimer_alarm_config_t alarm_config;

    uint64_t count;

    /* Configure a universal timer */
    ESP_LOGI("GPTIMER_ALARM", "Configure a universal timer");                                 /* Create a universal timer handle */                                 
    
    gptimer_handle_t g_tim = NULL;
    gptimer_config_t g_tim_handle = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,                                             /* Select the timer clock source */   
        .direction = GPTIMER_COUNT_UP,                                                  /* Count-up mode */
        .resolution_hz = resolution,                                                    /* Counter resolution */
    };
        gptimer_event_callbacks_t g_tim_callbacks = {
        .on_alarm = gptimer_callback,                                                   /* Register user callback function */ 
    };
    alarm_config.alarm_count = 1000000;                                                 /* Alarm target count value */
    ESP_ERROR_CHECK(gptimer_new_timer(&g_tim_handle, &g_tim));                          /* Create a new general-purpose timer and return the handle */

    queue = xQueueCreate(10, sizeof(gptimer_event_t));                                  /* Create a queue to carry events */

    if (!queue)
    {
        ESP_LOGE("GPTIMER_ALARM", "Failed to create a queue");                                /* Failed to create a queue */

        return;
    }

    /* Set and get the count value */
    ESP_LOGI("GPTIMER_ALARM", "set upCount value");
    ESP_ERROR_CHECK(gptimer_set_raw_count(g_tim, counts));                              /* Set the count value */
    ESP_LOGI("GPTIMER_ALARM", "GetCount value");
    ESP_ERROR_CHECK(gptimer_get_raw_count(g_tim, &count));                              /* Get the count value */
    ESP_LOGI("GPTIMER_ALARM", "Timer Count value: %llu", count);

    /* Register event callback function */
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(g_tim, &g_tim_callbacks, queue));  /* Configure the universal timer callback function */

    /* Set alarm action */
    ESP_LOGI("GPTIMER_ALARM", "Enable general-purpose timer");
    ESP_ERROR_CHECK(gptimer_enable(g_tim));                                             /* Enable general-purpose timer */
    ESP_ERROR_CHECK(gptimer_set_alarm_action(g_tim, &alarm_config));                    /* Configure the general-purpose timer alarm event */
    ESP_ERROR_CHECK(gptimer_start(g_tim));                                              /* Start the universal timer */
}

/**
 * @brief       Timer callback function
 * @param       none
 * @retval      none
 */
bool IRAM_ATTR gptimer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    queue = (QueueHandle_t)user_data;

    /* Extract the count value from the event data */
    gptimer_event_t ele = {
        .event_count = edata->count_value
    };

    /* Optional: Send event data to other tasks through operating system queues */
    xQueueSendFromISR(queue, &ele, &high_task_awoken);
    
    /* Reconfigure alarm values */ 
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = edata->alarm_value + 1000000, /* Alarm within the next 1 second */ 
    };
    gptimer_set_alarm_action(timer, &alarm_config);
    
    /* Return whether a context switch is required at the end of the ISR */ 
    return high_task_awoken == pdTRUE;
}
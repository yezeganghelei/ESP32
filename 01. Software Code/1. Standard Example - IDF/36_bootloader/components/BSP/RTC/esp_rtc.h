/**
 ****************************************************************************************************
 * @file        rtc.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       RTCDriver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __ESP_RTC_H
#define __ESP_RTC_H

#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <sys/time.h>

/* hour间结构体, 包括YearmoondayweekhourpointSecond等信息 */
typedef struct
{
    uint8_t hour;       /* hour */
    uint8_t min;        /* point */
    uint8_t sec;        /* Second */
    /* 公历Yearmoondayweek */
    uint16_t year;      /* Year */
    uint8_t  month;     /* moon */
    uint8_t  date;      /* day */
    uint8_t  week;      /* week */
} _calendar_obj;

extern _calendar_obj calendar;      /* hour间结构体 */

/* Function declaration */
void rtc_set_time(int year,int mon,int mday,int hour,int min,int sec);  /* 设置hour间 */
void rtc_get_time(void);                                                /* 获取hour间 */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day);        /* 获取week几 */

#endif
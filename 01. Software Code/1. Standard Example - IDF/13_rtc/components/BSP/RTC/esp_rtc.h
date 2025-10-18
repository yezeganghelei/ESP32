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

/* hourbetween结构体, includeYearmoondayweekhourpointSecondWait for information */
typedef struct
{
    uint8_t hour;       /* hour */
    uint8_t min;        /* point */
    uint8_t sec;        /* Second */
    /* Gregorian calendarYearmoondayweek */
    uint16_t year;      /* Year */
    uint8_t  month;     /* moon */
    uint8_t  date;      /* day */
    uint8_t  week;      /* week */
} _calendar_obj;

extern _calendar_obj calendar;      /* hourbetween结构体 */

/* Function declaration */
void rtc_set_time(int year,int mon,int mday,int hour,int min,int sec);  /* set uphourbetween */
void rtc_get_time(void);                                                /* Gethourbetween */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day);        /* GetweekSeveral */

#endif
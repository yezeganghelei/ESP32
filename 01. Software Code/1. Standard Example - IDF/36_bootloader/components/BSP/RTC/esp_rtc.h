/**
 ****************************************************************************************************
 * @file        rtc.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       RTC driver code
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

/* Time structure, including year, month, day, week, hour, minute, second, etc. */
typedef struct
{
    uint8_t hour;       /* hour */
    uint8_t min;        /* minute */
    uint8_t sec;        /* second */
    /* Gregorian year, month, day, and week */
    uint16_t year;      /* Year */
    uint8_t  month;     /* month */
    uint8_t  date;      /* day */
    uint8_t  week;      /* week */
} _calendar_obj;

extern _calendar_obj calendar;      /* Time structure */

/* Function declaration */
void rtc_set_time(int year,int mon,int mday,int hour,int min,int sec);  /* Set time */
void rtc_get_time(void);                                                /* Get time */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day);        /* Get day of week */

#endif
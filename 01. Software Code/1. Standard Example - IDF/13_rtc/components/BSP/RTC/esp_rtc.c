/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file rtc.c
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief RTC driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "esp_rtc.h"

_calendar_obj calendar;         /* Time structure */

/**
 * @brief       RTC set time
 * @param       year    :Year
 * @param       mon     : Month
 * @param       mday    :day
 * @param       hour    :hour
 * @param       min     : Minute
 * @param       sec     : Second
 * @retval      none
 */
void rtc_set_time(int year,int mon,int mday,int hour,int min,int sec)
{
    struct tm datetime;
    /* Set time */
    datetime.tm_year = year - 1900;
    datetime.tm_mon = mon - 1;
    datetime.tm_mday = mday;
    datetime.tm_hour = hour;
    datetime.tm_min = min;
    datetime.tm_sec = sec;
    datetime.tm_isdst = -1;
    /* Get total seconds since 1970-01-01 */
    time_t second = mktime(&datetime);
    struct timeval val = { .tv_sec = second, .tv_usec = 0 };
    /* Set the current time */
    settimeofday(&val, NULL);
}

/**
 * @brief       Get the current time
 * @param       none
 * @retval      none
 */
void rtc_get_time(void)
{
    struct tm *datetime;
    time_t second;
    /* Return the elapsed seconds since 1970-01-01 00:00:00 UTC */
    time(&second);
    datetime = localtime(&second);

    calendar.hour = datetime->tm_hour;          /* Hours */
    calendar.min = datetime->tm_min;            /* Minutes */
    calendar.sec = datetime->tm_sec;            /* Seconds */
    /* Gregorian calendar year, month, day, week */
    calendar.year = datetime->tm_year + 1900;   /* Year */
    calendar.month = datetime->tm_mon + 1;      /* Month */
    calendar.date = datetime->tm_mday;          /* Day */
    /* Week */
    calendar.week = rtc_get_week(calendar.year, calendar.month, calendar.date);
}

/**
 * @brief       Convert year, month, day, hour, minute, second to seconds
 *   @note      Given a Gregorian date, get the day of week (epoch: 0 AD, March 1; any future date yields the correct weekday)
 *              Calculated using Kim Larson's formula; see this post for the principle:
 *              https://www.cnblogs.com/fengbohello/p/3264300.html
 * @param       syear : years
 * @param       smon  : Month
 * @param       sday  : date
 * @retval      0, Sunday; 1 ~ 6: Monday ~ Saturday
 */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day)
{
    uint8_t week = 0;

    if (month < 3)
    {
        month += 12;
        --year;
    }

    week = (day + 1 + 2 * month + 3 * (month + 1) / 5 + year + (year >> 2) - year / 100 + year / 400) % 7;
    return week;
}
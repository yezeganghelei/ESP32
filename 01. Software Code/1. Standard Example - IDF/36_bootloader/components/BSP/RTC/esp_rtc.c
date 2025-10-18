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

_calendar_obj calendar;         /* hour间结构体 */

/**
 * @brief       RTCSet time
 * @param       year    :Year
 * @param       mon     :moon
 * @param       mday    :day
 * @param       hour    :hour
 * @param       min     :point
 * @param       sec     :Second
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
    /* Get1970.1.1以来的总Second数 */
    time_t second = mktime(&datetime);
    struct timeval val = { .tv_sec = second, .tv_usec = 0 };
    /* 设置当前hour间 */
    settimeofday(&val, NULL);
}

/**
 * @brief Get the current time
 * @param none
 * @retval None
 */
void rtc_get_time(void)
{
    struct tm *datetime;
    time_t second;
    /* Returns the elapsed time (seconds) since (1970.1.1 00:00:00 UTC) */
    time(&second);
    datetime = localtime(&second);

    calendar.hour = datetime->tm_hour;          /* hour */
    calendar.min = datetime->tm_min;            /* point */
    calendar.sec = datetime->tm_sec;            /* Second */
    /* 公历Yearmoondayweek */
    calendar.year = datetime->tm_year + 1900;   /* Year */
    calendar.month = datetime->tm_mon + 1;      /* moon */
    calendar.date = datetime->tm_mday;          /* day */
    /* week */
    calendar.week = rtc_get_week(calendar.year, calendar.month, calendar.date);
}

/**
 * @brief       将YearmoondayhourpointSecond转换成Second钟数
 *   @note      输入公历date得到星期(起始hour间为: A.D.0Year3moon1Day begins, 输入往后的任何date, You can get the correct day of the week)
 *              use 基姆拉尔森calculate公式 calculate, See this post for principle explanation:
 *              https://www.cnblogs.com/fengbohello/p/3264300.html
 * @param       syear : Year份
 * @param       smon  : moon份
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
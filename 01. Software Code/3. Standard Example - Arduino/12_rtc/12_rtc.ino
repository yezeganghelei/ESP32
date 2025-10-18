/**
 ******************************************************************************
 * @file     12_rtc.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    RTC experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experimentPurpose：studyRTCUse of peripherals

 * Hardware resources and pin assignments：
 * 1,   UART0 --> ESP32S3 IO
 *       TXD0 --> IO43
 *       RXD0 --> IO44
 * 2,  XL9555 --> ESP32S3 IO
 *        SCL --> IO42
 *        SDA --> IO41
 *        INT --> IO0(Jumper cap connection) 
 * 3, SPI_LCD --> ESP32S3 IO / XL9555
 *         CS --> IO21
 *        SCK --> IO12
 *        SDA --> IO11
 *         DC --> IO40(Jumper cap connection)
 *        PWR --> XL9555_P13
 *        RST --> XL9555_P12

 * experimentPhenomenon：
 * 1, LCDshowRTCreal time clockYearmoondayHours, minutes, seconds, day of the weekinformation

 * Things to note：
 * 1, Need to useESP32TimeLibrary,Specific operations：Select in software"project"-->"loadLibrary"-->"ManageLibrary"-->enter"ESP32Time"Just install it

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include <ESP32Time.h>    /* Requires installationESP32TimeLibrary */

ESP32Time rtc;
uint8_t tbuf[100];        /* StoreRTCinformation */

/**
 * @brief    When the program starts executing，Will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    uart_init(0, 115200);   /* serial port0initialization */
    xl9555_init();          /* IOexpansion chipinitialization */
    lcd_init();             /* LCDinitialization */
    rtc.setTime(00, 51, 17, 1, 12, 2023);  /* 2023Year12moon1day17:52:00 */
    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "RTC TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
}

/**
 * @brief    cyclefunction，Usually put the main body of the program or statements that need to be refreshed continuously.
 * @param    none
 * @retval   none
 */
void loop() 
{
    struct tm timeinfo = rtc.getTimeStruct();
    /* according totime.hin header filetmAdjust the definition of the structure */
    sprintf((char *)tbuf, "Time:%02d:%02d:%02d", timeinfo.tm_hour - 1, timeinfo.tm_min, timeinfo.tm_sec);     
    lcd_show_string(30, 130, 210, 16, LCD_FONT_16, (char *)tbuf, RED);
    sprintf((char *)tbuf, "Date:%04d-%02d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
    lcd_show_string(30, 150, 210, 16, LCD_FONT_16, (char *)tbuf, RED);
    sprintf((char *)tbuf, "Week:%d", timeinfo.tm_wday);
    lcd_show_string(30, 170, 210, 16, LCD_FONT_16, (char *)tbuf, RED);

    delay(1000);

    /* ESP32Timeotherfunctioninterface,Can use it yourselfSerial.printlnfunctionPrint
    getTime()           //  (String) 15:24:38
    getDate()           //  (String) Sun, Jan 17 2021
    getDate(true)       //  (String) Sunday, January 17 2021
    getDateTime()       //  (String) Sun, Jan 17 2021 15:24:38
    getDateTime(true)   //  (String) Sunday, January 17 2021 15:24:38
    getTimeDate()       //  (String) 15:24:38 Sun, Jan 17 2021
    getTimeDate(true)   //  (String) 15:24:38 Sunday, January 17 2021

    getMicros()         //  (unsigned long) 723546
    getMillis()         //  (unsigned long) 723
    getEpoch()          //  (unsigned long) 1609459200
    getLocalEpoch()     //  (unsigned long) 1609459200 // local epoch without offset
    getSecond()         //  (int)     38    (0-59)
    getMinute()         //  (int)     24    (0-59)
    getHour()           //  (int)     3     (0-12)
    getHour(true)       //  (int)     15    (0-23)
    getAmPm()           //  (String)  pm
    getAmPm(false)      //  (String)  PM
    getDay()            //  (int)     17    (1-31)
    getDayofWeek()      //  (int)     0     (0-6)
    getDayofYear()      //  (int)     16    (0-365)
    getMonth()          //  (int)     0     (0-11)
    getYear()           //  (int)     2021
    */
}
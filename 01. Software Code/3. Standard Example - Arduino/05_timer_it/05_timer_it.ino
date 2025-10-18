/**
 ******************************************************************************
 * @file     05_timer_it.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    timer interrupt experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Purpose of the experiment：studyTIMERUsage of interrupt function of timer peripherals

 * Hardware resources and pin assignments：
 * 1, LED --> ESP32S3 IO
 *    LED --> IO1

 * Experimental phenomenon：
 * 1, LEDIndicator interval500msstate flip

 * Things to note：
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "tim.h"
#include "led.h"

/**
 * @brief    When the program starts executing，will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    led_init();                 /* LEDinitialization */
    timx_int_init(5000, 8000);  /* Timer initialization, time time is 500ms */
}

/**
 * @brief    cyclefunction，Usually put the main body of the program or statements that need to be refreshed continuously.
 * @param    none
 * @retval   none
 */
void loop() 
{
    /* A dead cycle,don't do things,Wait for the timer interrupt to trigger */
    delay(1000);
}
/**
 ******************************************************************************
 * @file     01_led.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    LEDLamp experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Purpose of the experiment：studyGPIOUse as output，How to light up oneLED

 * Hardware resources and pin assignments： 
 * 1, LED --> ESP32S3 IO
 *    LED --> IO1

 * Experimental phenomenon：
 * 1, LEDDeng every500msflash

 * Things to note：
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "led.h"

/**
 * @brief    When the program starts executing，Will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    led_init();   /* LEDinitialization */
}

/**
 * @brief    cyclefunction，Usually put the main body of the program or statements that need to be refreshed continuously.
 * @param    none
 * @retval   none
 */
void loop() 
{
    LED(0);       /* The macro function isled.hDefined,Equivalent todigitalWrite(LED, LOW),LEDPin output is connected to low level,light up */
    delay(500);   /* Delay 500 milliseconds */
    LED(1);       /* The macro function isled.hDefined,Equivalent todigitalWrite(LED, HIGH),LEDPin output is connected to high level,Extinguish */
    delay(500);   /* Delay 500 milliseconds */

    /* above4The effect of line code implementation can also be achieved using the following two lines of code
    LED_TOGGLE(); // LED status flip
    delay(500);   // Delay 500 milliseconds
    */
}
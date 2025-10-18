/**
 ******************************************************************************
 * @file     02_key.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    KEYKey Experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Purpose of the experiment：studyGPIOUse as input，DetectionKEYButton status

 * Hardware resources and pin allocation： 
 * 1, LED --> ESP32S3 IO
 *    LED --> IO1
 * 2, KEY --> ESP32S3 IO
 *    KEY --> IO0

 * Experimental phenomenon：
 * 1, PressKEY，LEDThe indicator light is on，NoPressKEY，LEDThe indicator light goes off

 * Things to note：
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "led.h"
#include "key.h"

/**
 * @brief    When the program starts executing，Will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    led_init();     /* LEDinitialization */
    key_init();     /* KEYinitialization */
}

/**
 * @brief    cyclefunction，Usually the main body of the program or the statement that needs to be refreshed continuously
 * @param    none
 * @retval   none
 */
void loop() 
{
    if (KEY == 0)     /* ReadKEYstate,ifPressKEY */
    {
        delay(10);

        if (KEY == 0)
        {
          LED(0);     /* The LED pin output is connected to a low level and illuminated */
        }
    }
    else              /* ReadKEYstate,ifKEYNoPress */
    {
        LED(1);       /* LEDPin output is connected to high level,Extinguish */
    }
}
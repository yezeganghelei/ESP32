/**
 ******************************************************************************
 * @file     03_exti.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    EXTIExternal interrupt experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose：Learn the use of external interrupts，DetectionKEYKey status

 * Hardware resources and pin assignments：
 * 1, LED --> ESP32S3 IO
 *    LED --> IO1
 * 2, KEY --> ESP32S3 IO
 *    KEY --> IO0

 * Experimental phenomenon：
 * 1, PressKEY，Trigger external interrupt，LEDThe indicator light status changes，From bright to extinction，Or from extinction to brightness

 * Things to note：
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "led.h"
#include "exti.h"

/**
 * @brief    When the program starts executing，Will callsetup()function，Usually used to initialize variables、functions etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    led_init();     /* LEDinitialization */
    exti_init();    /* External interrupt pininitialization */
}

/**
 * @brief    cyclefunction，Usually put the main body of the program or statements that need to be refreshed continuously.
 * @param    none
 * @retval   none
 */
void loop() 
{
    LED(led_state); /* The light is turned offled_stateDetermined by value,led_stateChanges inkey_isrimplemented in function */
}
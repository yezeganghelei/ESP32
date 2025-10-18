/**
 ******************************************************************************
 * @file 07_led_pwm.ino
 * @author team()
 * @version V1.0
 * @date 2023-12-01
 * @brief LED PWMexperiment
 * @license Copyright (c) 2020-2032,
 ******************************************************************************

 * Experiment purpose: studyLED PWMUse of peripherals

 * Hardware resources and pin allocation:
 * 1, LED --> ESP32S3 IO
 * LED --> IO1

 *experiment phenomenon:
 * 1, LEDIndicator light achieves breathing light effect, From dark to bright, From light to darker

 *Things to note:
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "pwm.h"

uint16_t g_ledpwmval = 0;     /* duty cycle value */
uint8_t g_dir = 1;            /* direction of change（1increase 0Reduce） */

/**
 * @brief    When the program starts executing，will callsetup()function，Usually used to initialize variables、functions etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    led_pwm_init(1000, 10);   /* LED PWMinitialization,PWMThe output frequency is1000HZ,The duty cycle resolution is10 */
}

/**
 * @brief    cyclefunction，Usually the main body of the program or the statement that needs to be refreshed continuously
 * @param    none
 * @retval   none
 */
void loop() 
{
    if (g_dir)
    {
        g_ledpwmval += 5;
    }
    else
    {
        g_ledpwmval -= 5;
    }

    if (g_ledpwmval > 1005)
    {
        g_dir = 0;
    }

    if (g_ledpwmval < 5)
    {
        g_dir = 1;
    }

    pwm_set_duty(g_ledpwmval);
    delay(10);
}
/**
 ******************************************************************************
 * @file     06_watch_dog.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    watchdog experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Purpose of the experiment：Learn to use watchdogs(Timer virtual watchdog function)

 * Hardware resources and pin assignments： 
 * 1,   KEY --> ESP32S3 IO
 *      KEY --> IO0
 * 2, UART0 --> ESP32S3 IO
 *     TXD0 --> IO43
 *     RXD0 --> IO44

 * experimental phenomenon：
 * 1, Serial portPrint runtime information，when the button is pressed，Running time becomes longer，lead tononeFa Zai1.2Feeding dogs within seconds，And entering the timer interrupt callbackfunctionPerform software reset operation in

 * Things to note：
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "uart.h"
#include "key.h"
#include "watchdog.h"

#define wdg_timeout 1200    /* Watchdog timer,1200ms */

/**
 * @brief    When the program starts executing，Will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    key_init();                           /* KEYinitialization */
    uart_init(0, 115200);                 /* Serial port0initialization */
    Serial.println("running setup");      /* Print iconic information Easy to view system start */

    wdg_init(wdg_timeout * 1000, 80);     /* initializationwatchdog,80Dividing frequency,Timer time1.2Second */
}

/**
 * @brief    cyclefunction，Usually put the main body of the program or statements that need to be refreshed continuously.
 * @param    none
 * @retval   none
 */
void loop() 
{
    Serial.println("running main loop");  /* Print iconic information Easy to view system start */

    timerWrite(wdg_timer, 0);             /* Reset timer (feeding dogs) */

    long looptime = millis();             /* passmillisThe function gets the number of milliseconds that have passed since the development board started running the current program. */

    while (!KEY)                          /* Pressing the button will delay 500ms, which will eventually cause the looptime to become 1.5 seconds. Before you can feed the dog, it will enter the timer interrupt callback function to reset. */
    {
        Serial.println("key pressed, delay_500ms");
        delay(500);
    }

    delay(1000);                          
    looptime = millis() - looptime;       /* Monitor process running time */

    Serial.print("loop time is = ");      /* Print process running time */
    Serial.println(looptime); 
}
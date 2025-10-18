/**
 ******************************************************************************
 * @file     09_iic_exio.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    IOExtended experiments
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose：studyIOexpansion chipXL9555The use of

 * Hardware resources and pin assignments：
 * 1,    LED --> ESP32S3 IO
 *       LED --> IO1
 * 2,  UART0 --> ESP32S3 IO
 *      TXD0 --> IO43
 *      RXD0 --> IO44
 * 3, XL9555 --> ESP32S3 IO
 *       SCL --> IO42
 *       SDA --> IO41
 *       INT --> IO0(Jumper cap connection) 

 * Experimental phenomenon：
 * 1, pressdevelopment boardKEY0andKEY1controlbuzzer working，pressKEY2andKEY3controlLEDThe light goes off

 * Things to note：
 * 1, Need to useJumper cap connectionIO0andIIC_INT

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "xl9555.h"
#include "led.h"
#include "uart.h"

/**
 * @brief    When the program starts executing，will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    led_init();             /* LEDinitialization */
    uart_init(0, 115200);   /* serial port0initialization */
    xl9555_init();          /* IOexpansion chipinitialization */

    xl9555_io_config(KEY0 | KEY1 | KEY2 | KEY3, IO_SET_INPUT);  /* Initialize the pin of the IO expansion chip used as a button to the input state */
    xl9555_io_config(BEEP, IO_SET_OUTPUT);                      /* initializationIOexpansion chipused as buzzercontrolThe pin is in the output state */
}

/**
 * @brief    cyclefunction，Usually the main body of the program or the statement that needs to be refreshed continuously
 * @param    none
 * @retval   none
 */
void loop() 
{
    if (!IIC_INT)   /* The port changes to low level */
    {
        if (xl9555_get_pin(KEY0) == 0)
        {
            Serial.printf("KEY0 is pressed, BEEP is on \r\n");
            xl9555_pin_set(BEEP, IO_SET_LOW);
        }

        if (xl9555_get_pin(KEY1) == 0)
        {
            Serial.printf("KEY1 is pressed, BEEP is off \r\n");
            xl9555_pin_set(BEEP, IO_SET_HIGH);
        }

        if (xl9555_get_pin(KEY2) == 0)
        {
            Serial.printf("KEY2 is pressed, LED is on \r\n");
            LED(0);
        }

        if (xl9555_get_pin(KEY3) == 0)
        {
            Serial.printf("KEY3 is pressed, LED is off \r\n");
            LED(1);
        }
    }

    delay(200);
}
/**
 ******************************************************************************
 * @file     10_iic_oled.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    OLEDshow experiments
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose：studyOLEDUse of modules

 * Hardware resources and pin allocation：
 * 1,  UART0 --> ESP32S3 IO
 *      TXD0 --> IO43
 *      RXD0 --> IO44
 * 2, XL9555 --> ESP32S3 IO
 *       SCL --> IO42
 *       SDA --> IO41
 *       INT --> IO0(Jumper cap connection) 
 * 3,   OLED --> ESP32S3 IO
 *   D0(SCL) --> IO40
 *   D1(SDA) --> IO41
 *        D2 --> IO6
 *        DC --> IO38

 * Experimental phenomenon：
 * 1, Set it upIICcommunication interfaceOLEDInsert the module into the lower left corner of the development boardOLED/CAMERAModule interface，OLEDThe module keeps showingASCIIcode and code value

 * Things to note：
 * 1, Need to select in the software"project"-->"loadLibrary"-->"add a.ZIPLibrary..."-->Select to the data package directory1compressed file package“esp8266-oled-ssd1306-master.zip”Just install it。

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "oled.h"
#include "uart.h"
#include "xl9555.h"

/**
 * @brief    When the program starts executing，Will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    uart_init(0, 115200);   /* serial port0initialization */
    xl9555_init();          /* IOexpansion chipinitialization */
    oled_init();            /* OLED module initialization */
}

/**
 * @brief    cyclefunction，Usually the main body of the program or the statement that needs to be refreshed continuously
 * @param    none
 * @retval   none
 */
void loop() 
{
    oled_show_demo();
}
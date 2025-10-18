/**
 ******************************************************************************
 * @file     20_camera.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Camera experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experimentPurpose：studyCameraThe use of

 * Hardware resources and pin assignments：
 * 1,     LED --> ESP32S3 IO
 *        LED --> IO1
 * 2,   UART0 --> ESP32S3 IO
 *       TXD0 --> IO43
 *       RXD0 --> IO44
 * 3,  XL9555 --> ESP32S3 IO
 *        SCL --> IO42
 *        SDA --> IO41
 *        INT --> IO0(Jumper cap connection) 
 * 4, SPI_LCD --> ESP32S3 IO / XL9555
 *         CS --> IO21
 *        SCK --> IO12
 *        SDA --> IO11
 *         DC --> IO40(Jumper cap connection)
 *        PWR --> XL9555_P13
 *        RST --> XL9555_P12
 * 5,  CAMERA --> ESP32S3 IO / XL9555
 *     OV_SCL --> 38
 *     OV_SDA --> 39
 *      VSYNC --> 47
 *       HREF --> 48
 *       PCLK --> 45
 *         D0 --> 4
 *         D1 --> 5
 *         D2 --> 6
 *         D3 --> 7
 *         D4 --> 15
 *         D5 --> 16
 *         D6 --> 17
 *         D7 --> 18
 *      RESET --> XL9535_P05
 *       PWDN --> XL9535_P04

 * experimentPhenomenon：
 * 1, LCDwill be displayedCameraWhat was filmed

 * Things to note：
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "uart.h"
#include "led.h"
#include "led.h"
#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "camera.h"

/**
 * @brief    When the program starts executing，Will callsetup()function，Usually used to initialize variables、functions etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    led_init();             /* LEDinitialization */
    uart_init(0, 115200);   /* serial port0initialization */
    xl9555_init();          /* IOexpansion chipinitialization */
    lcd_init();             /* LCDinitialization */

    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "CAMERA TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);

    while (camera_init())   /* Camerainitialization */
    {
        lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "Please check camera!", RED);
        delay(200);
        lcd_fill(30, 110, 200, 126, WHITE);
        delay(200);
    }        
}

/**
 * @brief    cyclefunction，Usually put the main body of the program or statements that need to be refreshed continuously.
 * @param    none
 * @retval   none
 */
void loop() 
{
    camera_capture_show();  /* existLCDDisplay data captured by camera */
}
/**
 ******************************************************************************
 * @file 19_iic_qma6100p.ino
 * @author team()
 * @version V1.0
 * @date 2023-12-01
 * @brief Three-axis accelerometer experiment
 * @license Copyright (c) 2020-2032,
 ******************************************************************************

 * Experiment purpose: learn the use of Three-axis accelerometer

 * Hardware resources and pin allocation:
 * 1, LED --> ESP32S3 IO
 * LED --> IO1
 * 2, UART0 --> ESP32S3 IO
 * TXD0 --> IO43
 * RXD0 --> IO44
 * 3, XL9555 --> ESP32S3 IO
 * SCL --> IO42
 * SDA --> IO41
 * INT --> IO0(Jumper cap connection)
 * 4, SPI_LCD --> ESP32S3 IO / XL9555
 * CS --> IO21
 * SCK --> IO12
 * SDA --> IO11
 * DC --> IO40(Jumper cap connection)
 * PWR --> XL9555_P13
 * RST --> XL9555_P12
 * 5, QMA6100P--> ESP32S3 IO
 * SCL --> IO40
 * SDA --> IO41
 * INT --> XL9555_P01

 *experiment phenomenon:
 * 1, LCDThe pitch angle of the three-axis acceleration sensor will be displayedpitchand rolling anglerolldata

 *Things to note:
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "led.h"
#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "qma6100p.h"
#include "imu.h"

float g_acc_data[3];
float g_angle_data[2];

/**
 * @brief display angle
 * @param x, y: coordinate
 * @param title: title
 * @param angle: angle
 * @retval none
 */
void user_show_angle(uint16_t x, uint16_t y, char *title, float angle)
{
    char buf[20];

    sprintf(buf,"%s%3.1f", title, angle);                     /* Format output */
    lcd_fill(x, y, x + 160, y + 16, WHITE);                   /* Clear last data(Maximum display20characters,20*8=160) */
    lcd_show_string(x, y, 160, 16, LCD_FONT_16, buf, BLUE);   /* display string */
}

/**
 * @brief    When the program starts executing，Will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    led_init();             /* LEDinitialization */
    uart_init(0, 115200);   /* Serial port0initialization */
    xl9555_init();          /* IOexpansion chipinitialization */
    lcd_init();             /* LCDinitialization */
    qma6100p_init();        /* Three-axis accelerometerinitialization */
    
    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "IMU TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
}

/**
 * @brief    cyclefunction，Usually put the main body of the program or statements that need to be refreshed continuously.
 * @param    none
 * @retval   none
 */
void loop() 
{
    qma6100p_read_acc_xyz(g_acc_data);
    acc_get_angle(g_acc_data, g_angle_data);

    user_show_angle(30, 130, "Pitch :", g_angle_data[0]);
    user_show_angle(30, 150, " Roll :", g_angle_data[1]);

    LED_TOGGLE();
    delay(500);
}
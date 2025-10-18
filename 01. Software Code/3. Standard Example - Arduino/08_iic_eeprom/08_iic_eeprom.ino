/**
 ******************************************************************************
 * @file 08_iic_eeprom.ino
 * @author team()
 * @version V1.0
 * @date 2023-12-01
 * @brief EEPROM experiment
 * @license Copyright (c) 2020-2032,
 ******************************************************************************

 * Experiment project: studyIIC peripherals are used, rightEEPROMDevices performs read and write operations

 * Hardware resources and pin allocation:
 * 1, KEY --> ESP32S3 IO
 * KEY --> IO0
 * 2, UART0 --> ESP32S3 IO
 * TXD0 --> IO43
 * RXD0 --> IO44
 * 3, EEPROM --> ESP32S3 IO
 * SCL --> IO42
 * SDA --> IO41

 *experiment phenomenon:
 * 1, TestIICAre there any on the bus24C02Devices, pressKEYWill write data toEEPROM, Every other1It will print out in secondsEEPROMof0Valid contents of the address starting to store

 *Things to note:
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */
 
#include "24c02.h"
#include "key.h"
#include "uart.h"

const uint8_t g_text_buf[] = {"ESP32S3 IIC TEST"};  /* String array to be written to 24c02 */
#define TEXT_SIZE   sizeof(g_text_buf)              /* TEXT string length */
uint8_t datatemp[TEXT_SIZE];                        /* fromEEPROMRead data */

/**
 * @brief    When the program starts executing，Will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    key_init();               /* KEYinitialization */
    uart_init(0, 115200);     /* serial port0initialization */
    at24c02_init();           /* initialization24CXX */
    
    while (at24c02_check())   /* Testless than24c02 */
    {
        Serial.println("24C02 Check Failed!");
        delay(500);
    }
    Serial.println("24C02 Ready!");
}

/**
 * @brief    cyclefunction，Usually put the main body of the program or statements that need to be refreshed continuously.
 * @param    none
 * @retval   none
 */
void loop() 
{
    at24c02_read(0, datatemp, TEXT_SIZE);                       /* from24C02of0Read from addressTEXT_SIZELength data */
    Serial.printf("The Data Readed Is:%s \r\n", datatemp);

    if (KEY == 0)
    {
        at24c02_write(0, (uint8_t *)g_text_buf, TEXT_SIZE);     /* Towards24C02of0Write at the addressTEXT_SIZELength data */
        Serial.printf("24C02 Write %s Finished! \r\n", g_text_buf);
    }

    delay(1000);
}
/**
 ******************************************************************************
 * @file     04_uart.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Serial port experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Purpose of the experiment：studyUARTUse of peripherals，Receive and send serial port data

 * Hardware resources and pin assignments： 
 * 1, UART0 --> ESP32S3 IO
 *     TXD0 --> IO43
 *     RXD0 --> IO44

 * experimental phenomenon：
 * 1, Print chip parameters，Open the serial port debugging assistant，Choose wellSerial port，Baud rate115200etc.，Send data on the serial assistant，ESP32S3Serial portAfter receiving, send back toSerial portAssistant display

 * Things to note：
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "uart.h"

uint32_t chip_id = 0;       /* chipID */

/**
 * @brief    When the program starts executing，Will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    uart_init(0, 115200);   /* Serial port0initialization */

    for(int i = 0; i < 17; i = i + 8) 
    {
        chip_id |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;       /* GetESP32chipMACaddress(6Byte)，ShouldaddressAlso available aschipID */
    }

    Serial.printf("ESP32 Chip model = %s Rev %d \n", ESP.getChipModel(), ESP.getChipRevision());   /* Printchip type and chipVersion number */
    Serial.printf("This chip has %d cores \n", ESP.getChipCores());     /* Print the number of cores of the chip */
    Serial.print("Chip ID: "); Serial.println(chip_id);                 /* PrintchipID */
    Serial.printf("CpuFreqMHz: %d MHz\n", ESP.getCpuFreqMHz());         /* Print chip main frequency */
    Serial.printf("SdkVersion: %s \n", ESP.getSdkVersion());            /* PrintSDKVersion */
}

/**
 * @brief    cyclefunction，Usually put the main body of the program or statements that need to be refreshed continuously.
 * @param    none
 * @retval   none
 */
void loop()
{
    Serial.println("Waitting for Serial Data  \n");   /* waitSerial portSent by assistantSerial portdata */

    while (Serial.available() > 0)                    /* When the serial port0Received data */
    {
        Serial.println("Serial Data Available...");   /* passSerial portMonitor notifies user */

        String serial_data;                           /* Store receivedSerial portdata */

        int c = Serial.read();                        /* Read one byteSerial portdata */
        while (c >= 0)
        {
            serial_data += (char)c;                   /* Store inserial_dataIn variables */
            c = Serial.read();                        /* Continue to read one byte of serial port data */
        }

        // serial_data = Serial.readString();            /* Use the received informationreadString()stored inserial_datavariable(Follow the front4lines of code have the same effect) */
        Serial.print("Received Serial Data: ");       /* Then output through the serial monitorserial_datavariablecontent */
        Serial.println(serial_data);                  /* To view information about serial_data variables */
    }

    delay(1000);
}
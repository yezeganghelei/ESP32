"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Serial port experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose：Learn the use of serial port（take over与发送）

 * Hardware resources and pin allocation： 
 * 1,   LED --> ESP32S3 IO
 *      LED --> IO1
 * 2, UART0 --> ESP32S3 IO
 *     TXD0 --> IO9
 *     RXD0 --> IO8

 * experimental phenomenon
 * 1, pass串口2and上位机对话，After receiving the string sent by the serial port assistant（End with a carriage return）back，Will be returned to the serial port assistant。
 *    At the same time every once in a while，pass串口2Output a piece of information，可以pass串口助手显示take over。

 * Things to note
 * 1, IO9andIO8Need to connect an externalUSBpassTTLSerial port module，Only then can you talk to the superior computer。

 ******************************************************************************

 ******************************************************************************
"""

from machine import UART,Pin
import time

if __name__ == '__main__':
    
    times = 0
    # Initialize the LED and output high level
    led = Pin(0,Pin.OUT,value = 1)
    # Initialize serial port 2
    uart = UART(2,115200,rx = 9,tx = 8,bits = 8, parity = None, stop = 1)           # Set serial port2and波特率

    while True:

        # Determine whether the message has been received
        if uart.any():
            print('Received data:')
            text = uart.read(128) #Receive 128 characters
            print(text) # Print the data received by serial port 2 through REPL
        else:
            times+=1
            if (times % 1000) == 0:
                uart.write('ESP32S3 Development Board Serial Port Experiment\r\n') # Send a piece of data
            elif (times % 200) == 0:
                uart.write('Please enter data\r\n')    # Send a piece of data
            elif (times % 30) == 0:
                led_state = led.value()
                led.value(not led_state)

        time.sleep_ms(1)
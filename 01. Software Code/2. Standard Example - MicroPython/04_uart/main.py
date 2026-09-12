"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Serial port experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose: Learn the use of the serial port (receive and send)

 * Hardware resources and pin allocation: 
 * 1,   LED --> ESP32S3 IO
 *      LED --> IO1
 * 2, UART0 --> ESP32S3 IO
 *     TXD0 --> IO9
 *     RXD0 --> IO8

 * Experimental phenomenon
 * 1, Communicate with the host computer through serial port 2. After receiving a string sent by the
 *    serial port assistant (ending with a carriage return), the program sends it back to the serial
 *    port assistant. At the same time, serial port 2 periodically outputs a piece of information,
 *    which the serial port assistant can display.

 * Things to note
 * 1, IO9 and IO8 need to be connected to an external USB-to-TTL serial port module before you can
 *    communicate with the host computer.

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
    uart = UART(2,115200,rx = 9,tx = 8,bits = 8, parity = None, stop = 1)           # Set serial port 2 and baud rate

    while True:

        # Determine whether the message has been received
        if uart.any():
            print('Received data:')
            text = uart.read(128) # Receive 128 characters
            print(text) # Print the data received by serial port 2 through the REPL
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
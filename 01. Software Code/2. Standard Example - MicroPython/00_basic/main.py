"""
 *********************************************************************************************
 * @file main.py
 * @author Team()
 * @version V1.0
 * @date 2023-12-01
 * @brief Basic Engineering
 * @license Copyright (c) 2020-2032,
 *********************************************************************************************

 * Experimental purpose: View ESP32-S3 internal resources

 * Hardware resources and pin allocation
 * none

 * Experimental phenomenon
 * 1. Print the internal resources of the ESP32S3 module on the serial port.

 * Notes
 * none

 *********************************************************************************************

 *********************************************************************************************
"""

import machine
import esp
import time
import micropython

""""
 * @brief program entry
 * @param None
 * @retval None
""""
if  __name__ == '__main__':
    
    esp.osdebug(1)                          # Open the original factory O/S debuginformation
    esp.osdebug(0)                          # Will the original factory O/S Debug information redirected to UART(0) 
    freq = machine.freq()                   # GetCPUcurrentOperating frequency
    print(f'Current system clock{freq}')              # Print CPU operating frequency
    print(f'internalflashsize{esp.flash_size()/1024/1024}MB') # Print FLASH size
    micropython.mem_info()                  # Print PSRAM information
    
    while True:
        
        time.sleep_ms(10)                   # Delay10ms
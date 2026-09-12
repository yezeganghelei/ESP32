"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    DS18B20 experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose: Read the temperature value from the DS18B20 sensor

 * Hardware resources and pin assignments: 
 * 1,     LED --> ESP32S3 IO
 *        LED --> IO1
 * 2,  XL9555 --> ESP32S3 IO
 *        SCL --> IO42
 *        SDA --> IO41
 * 3, SPI_LCD --> ESP32S3 IO / XL9555
 *         CS --> IO21
 *        SCK --> IO12
 *        SDA --> IO11
 *         DC --> IO40(Jumper cap connection)
 *        PWR --> XL9555_P13
 *        RST --> XL9555_P12
 * 4, DS18B20 --> ESP32S3 IO
 *         DQ --> IO0(Jumper cap connection)

 * Experiment phenomenon
 * 1, After the program is downloaded successfully, the LCD display updates the temperature value in
 *    real time.
 * 2, The LED flashes, indicating that the program is running.

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,SPI,I2C
import onewire, ds18x20
import atk_xl9555 as io_ex
import atk_lcd as lcd
import time

"""
 * @brief program entry
 * @param none
 * @retval None
"""
if __name__ == '__main__':

    # Initialize the LED and output high level
    led = Pin(1,Pin.OUT,value = 1)
    # IIC initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    # XL9555 initialization
    xl9555 = io_ex.init(i2c0)
    
    # Reset LCD
    xl9555.write_bit(io_ex.SLCD_RST,0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.SLCD_RST,1)
    time.sleep_ms(100)
    
    # Initialize SPI
    spi = SPI(2,baudrate = 80000000, sck = Pin(12), mosi = Pin(11), miso = Pin(13))
    # Initialize LCD; lcd = 0 for a 2.4-inch screen, lcd = 1 for a 1.3-inch SPI LCD screen;
    display = lcd.init(spi,dc = Pin(40,Pin.OUT,Pin.PULL_UP,value = 1),cs = Pin(21,Pin.OUT,Pin.PULL_UP,value = 1),dir = 1,lcd = 0)
    # Turn on the backlight
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    # Display experiment information
    display.string(30, 50, 240, 32, 32, "ESP32-S3",lcd.RED)
    display.string(30, 80, 240, 24, 24, "DS18B20 TEST",lcd.RED)
    display.string(30, 110, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
    display.string(30, 130, 200, 16, 16, "TEMPERATE: 00.00C", lcd.BLUE)
    # Initialize DS18B20
    ds_18b20 = ds18x20.DS18X20(onewire.OneWire(Pin(0)))

    """
     * @brief       Read the ds18x20 temperature value
     * @param       none
     * @retval      none
    """
    def read_sensor():
        # Discover the device
        roms = ds_18b20.scan()
        # Get value
        ds_18b20.convert_temp()
        for rom in roms:
            temp = ds_18b20.read_temp(rom)
            if isinstance(temp, float):
                temp = round(temp, 2)
                return temp

    while True:
        # Display the temperature value
        umber = float(read_sensor())
        display.num(30 + 11 * 8,130,int(umber),2,16,lcd.RED)
        display.num(30 + 14 * 8,130,int(umber * 100 % 100),2,16,lcd.RED)
        led_state = led.value()
        led.value(not led_state)
        time.sleep(1)
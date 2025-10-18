[[EN]](./lvgl_wificonfig_en.md)

# ESP32 LittlevGL Wi-Fi Configuration Example

## Example environment

- hardware:
	* [ESP32\_LCD\_EB\_V1](https://github.com/espressif/esp-dev-kits/blob/master/esp32-lcdkit/docs/ESP32_LCDKit_guide_cn.md) development board (this example needs to be used with [ESP32 DevKitC](https://docs.espressif.com/projects/esp-idf/en/stable/hw-reference/modules-and-boards.html#esp32-devkitc-v4) development board)
	* Screen (2.8 inch, 240*320 pixel, ILI9341 LCD + XPT2046 Touch)
- software：
	* [esp-iot-solution](https://github.com/espressif/esp-iot-solution)
	* [LittlevGL GUI](https://lvgl.io/)

Connection diagram：

<div align="center"><img src="../../../docs/_static/hmi_solution/lcd_connect.jpg" width = "700" alt="lcd_connect" align=center /></div>  

Default pin connections:

Name | Pin
-------- | -----
CLK | 22
MOSI | 21
MISO | 27
CS(LCD) | 5
DC | 19
RESET | 18
LED | 23
CS(Touch) | 32
IRQ | 33

## Run the example

- Enter the `examples/hmi/lvgl_wificonfig` directory
- Run `make defconfig`(Make) or `idf.py defconfig`(CMake) to use the default configuration
- run `make menuconfig`(Make) or `idf.py menuconfig`(CMake) Configure burning related configurations
- run `make -j8 flash`(Make) or `idf.py flash`(CMake) compile、Burn program to device

## #Example results

<div align="center"><img src="../../../docs/_static/hmi_solution/littlevgl/lvgl_wificonfig0.jpg" width = "700" alt="lvgl_wificonfig" align=center /></div>  

<div align="center"><img src="../../../docs/_static/hmi_solution/littlevgl/lvgl_wificonfig1.jpg" width = "700" alt="lvgl_wificonfig" align=center /></div>
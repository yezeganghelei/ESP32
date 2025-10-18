/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name：IOExtended experiments
 * Experimental platform： ESP32-S3 Development Board
 * Experimental purpose: Learn to use IIC to drive XL9555 (16-bit IO expansion chip)

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2 Independent buttons
     KEY0(XL9555) - IO1_7
     KEY1(XL9555) - IO1_6
     KEY2(XL9555) - IO1_5
     KEY3(XL9555) - IO1_4
     XL9555_INT   - IO40
 * 3 Buzzer
     BEEP - IO0_3
 * 4 XL9555
     IIC_SCL - IO42
     IIC_SDA - IO41

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 In this experiment, the buzzer is turned on by the KEY0 button, and the buzzer is turned off by the KEY1 button; the LED light is turned on by the KEY2 button, and the LED light is turned on.
     By pressing the keyKEY3To controlLEDLights off。
 * 2 Serial printingExperimental phenomenon，at the same time，When pressing different keys, the corresponding experimental information will be printed through the serial port.。

 ***************************************************************************************************
 * Things to note
 * 1 The baud rate of the computer serial debugging assistant must be 115200
 * 2 Please useXCOM/SSCOMSerial port debugging assistant,Other serial assistants may controlDTR/RTSlead toMCUcomplexBit/The program does not run
 * 3 Serial printingExperimental phenomenon
 * 4 Please useUSBThe wire is connected toDevelopment BoardOnUART,And test this routine after connecting to the computer（Please select the correct port，Different devices will have different ports！）
 
 ***********************************************************************************************************
 * Company Name：（）
 * telephone number：020-38271790
 * faxNumbercode：020-36773971
 * Company website：
 * Purchase address：zhengdianyuanzi.tmall.com
 * Technical forum: http:///forum.php
 * Latest information: /docs/index.html

 * Online video：
 * B Site video：space.bilibili.com/394620890
 * male All Number：mp.weixin.qq.com/s/y--mG3qQT8gop0VRuER9bw
 * Douyin: douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 ***********************************************************************************************************
 */
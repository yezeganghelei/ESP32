/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name：RMTInfrared reception experiment
 * Experimental platform： ESP32-S3 Development Board
 * Purpose of the experiment：Learn infraredNumber接收并解code的过程

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2 button
     BOOT -  IO0
 * 3 Serial port0 (Development Board needs to be connected to the computer)
 * 4 Infrared Receive Pin - IO2

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 This experiment startsLCDAfter displaying some information on it，That is, we enter and wait for infrared triggering，If the correct infrared signal is receivedNumber，
     Then decode and display the key value and the meaning represented on the LCD.
 * 2 Serial port prints the key value converted to decimal, and print hexadecimal data (Order).

 ***************************************************************************************************
 * Things to note
 * 1 The baud rate of the computer serial debugging assistant must be 115200
 * 2 This routine only supportsMCUscreen，Not supportedRGBscreen

 ***********************************************************************************************************
 * male department name: ()
 * TelephoneNumbercode：020-38271790
 * faxNumbercode：020-36773971
 * Company website：
 * Purchase address：zhengdianyuanzi.tmall.com
 * Technical forum: http:///forum.php
 * Latest information: /docs/index.html

 * Online video:
 * Site B video: space.bilibili.com/394620890
 * male All Number：mp.weixin.qq.com/s/y--mG3qQT8gop0VRuER9bw
 * Douyin: douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 ***********************************************************************************************************
 */
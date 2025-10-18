/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name：RMTInfrared transmission experiment
 * Experimental platform： ESP32-S3 Development Board
 * Experimental purpose: Learn the process of sending, receiving and decoding infrared signals

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2 button
     BOOT -  IO0
 * 3 Serial port0 (Development BoardneedandConnect to computer)
 * 4 Infrared send pin - IO8（BundleP3Port'sAINandRMTConnect with jumper cap）
 * 5 infrared receiver pin - IO2

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 This experiment startsLCDAfter displaying some information on it，Infrared send。Through infrared emitter（IR1）Send written hexadecimal instruction，
     Then the infrared receiver tube（IR2）take over。Finally displayed inLCDshowscreensuperior，Same，The serial port will also print the corresponding information。
 * 2 Serial port prints the key value converted to decimal, and print hexadecimal data (Order).

 ***************************************************************************************************
 * Things to note
 * 1 The baud rate of the computer serial debugging assistant must be 115200
 * 2 This routine only supports MCU screen, not RGB screen

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
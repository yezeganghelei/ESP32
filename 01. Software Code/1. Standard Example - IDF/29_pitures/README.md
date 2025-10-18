/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name：Image display experiment
 * Experiment platform: ESP32-S3 Development Board
 * Purpose of experiment：Learning inESP32-S3The aboveImage displaymethod,Implement a simple digital photo frame.

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2 Independent buttons
     KEY0(XL9555) - IO1_7
     KEY1(XL9555) - IO1_6
     KEY3(XL9555) - IO1_4
 * 3 XL9555
     IIC_SCL - IO42
     IIC_SDA - IO41
 * 4 ALIENTEK 1.3/2.4-inch SPILCD module
 * 5 SDCard,passSPI2drive 

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 BookexperimentCheck the font library first when booting，Then testSDDoes the card exist?，ifSDCardexist，Start searchingSDCardIn the root directoryPICTUREFolders，
 * If found, the image file below the folder is displayed (supports bmp, jpg, jpeg, png or gif formats), and display it cycle by pressing KEY0 and KEY1 to quickly
 *   Quickly browse the next oneandPrevious，KEY3The button is used to pause/Continue playing，DS1Used to indicate whether it is currently in a pause state。ifnot foundPICTURE
 *   Folders/Any picture file，Then the prompt is an error。
 * 2 LED flashes, indicating that the program is running

 ***************************************************************************************************
 * Things to note
 * USART1The communication baud rate is115200
 * Please useXCOMSerial port debugging assistant，Other serial port software may controlDTR、RSTlead toMCUReset、The program does not run
 * Development BoardIO4Pin connection serial portTTLOn the moduleRXD，IO5Pin connection serial portTTLOn the moduleTXD，The module is then connected to the computer via a data cable
 * Need to beSDCardCorrectly insert the onboardSDCard slot，Only then can this experimental routine be run normally

 ***********************************************************************************************************
 * Company Name：（）
 * telephone number：020-38271790
 * Fax Number: 020-36773971
 * Company website：
 * Purchase address：zhengdianyuanzi.tmall.com
 *Technical forum: http:///forum.php
 * Latest information: /docs/index.html

 * Online video：
 * B Site video：space.bilibili.com/394620890
 * male All Number：mp.weixin.qq.com/s/y--mG3qQT8gop0VRuER9bw
 * Douyin: douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 ***********************************************************************************************************
 */
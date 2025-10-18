/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name: SDCard experiment
 * Experimental platform： ESP32-S3 Development Board
 * Purpose of experiment：studySDIOdriveSDUse of the card

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2 button
     BOOT - IO0 
 * 3 1.3/2.4inchSPILCDModule
 * 4 SDCard(SDIOdrive)

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 After the program runs，If initializedSDCardsuccess，It will be through the serial port orVsCodeTerminal outputSDCardRelated parameters，And inLCDShown onSDCardTotal and remaining capacity
 * 2 LED flashes, indicating that the program is running

 ***************************************************************************************************
 * Things to note
 * USART1The communication baud rate is115200
 * Please useXCOMSerial port debugging assistant，Other serial port software may controlDTR、RSTlead toMCUReset、The program does not run
 * Development BoardIO4Pin connection serial portTTLOn the moduleRXD，IO5Pin connection serial portTTLOn the moduleTXD，The module is then connected to the computer via a data cable
 * Need to beSDCard correctly inserts the onboard SDCard slot, only then can this experimental routine be run normally

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
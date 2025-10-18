/**
 ***************************************************************************************************
 * Introduction to the experiment
 *Experiment name: Recorder experiment
 * Experiment platform: ESP32-S3 Development Board
 * Purpose of experiment：studyES8388andESP32S3 I2SUse of functions，Implement a simpleWAVrecorder

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2 independent buttons
     KEY0(XL9555) - IO1_7
     KEY1(XL9555) - IO1_6
     KEY2(XL9555) - IO1_5
     KEY3(XL9555) - IO1_4
 * 3 XL9555
     IIC_SCL - IO42
     IIC_SDA - IO41
 * 4 1.3/2.4-inch SPILCD module
 * 5 SDCard,passSPI2drive 
 * 6 ES8388 Audio CODEC Chip

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 BookexperimentTurn onback，Initialize each peripheral first，Then check if the font exists，If the detection is not problem，RetestSDCardIs the root directory
     existRECORDERfolder, Create if it does not exist, If creation fails, An error is reported. FoundSDCard's RECORDERfolder
     After that, the recording mode is entered (including configuring ES8388 and I2S, etc.), and the collected audio can be heard on the headset (or speaker).
     KEY0For start/Pause recordingsound，KEY2Used to save and stop recordingsound，KEY3Used to play the last recording。
 * 2 LEDFlashing，Indicates the running status of the program。LEDLong bright，Indicates the recording running status。

 ***************************************************************************************************
 * Things to note
 * USART1The communication baud rate is115200
 * Please useXCOMSerial port debugging assistant，Other serial port software may controlDTR、RSTlead toMCUReset、The program does not run
 * Need to beSDCard correctly inserts the onboard SDcard slot, only then can this experimental routine be run normally

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
/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name：EEPROMexperiment
 * Experiment platform: ESP32-S3 Development Board
 * Purpose of experiment：studyESP32drive24C02accomplishEEPROMData reading and writing

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2 Independent buttons
     KEY0(XL9555)    - IO1_7
     KEY1(XL9555)    - IO1_6
 * 3 24C02
     IIC_SCL - IO42
     IIC_SDA - IO41

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 BookexperimentpassKEY0Press the button to control24C02Data writing，By pressing the keyKEY1To control24C02Data reading。
 * 2 Serial port printingExperimental phenomenon，at the same time，不同的按键按下也会passSerial port printing相应的experimentinformation。

 ***************************************************************************************************
 * Things to note
 * 1 ComputerSerial port debugging assistantThe baud rate must be115200
 * 2 Please useXCOM/SSCOMSerial port debugging assistant,Other serial assistants may controlDTR/RTSlead toMCUReset/The program does not run
 * 3 Serial port printingExperimental phenomenon
 * 4 Please useUSBThe wire is connected toDevelopment BoardonUART,And test this routine after connecting to the computer（Please select the correct port，Different devices will have different ports！）
 
 ***********************************************************************************************************
 * maleName of the company：（）
 * TelephoneNumbercode：020-38271790
 * Fax Number: 020-36773971
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
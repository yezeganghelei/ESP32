/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name：soundLe player experiment
 * experimentplatform： ESP32-S3 Development board
 * Purpose of experiment：studySDIOdriveSDUse of the card

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2 Independent buttons
     KEY0(XL9555) - IO1_7
     KEY1(XL9555) - IO1_6
     KEY2(XL9555) - IO1_5
     KEY3(XL9555) - IO1_4
 * 3 XL9555
     IIC_SCL - IO42
     IIC_SDA - IO41
 * 4 1.3/2.4-inch SPILCD module
 * 5 SDCard,passSPI2drive 
 * 6 ES8388 audio CODEC chip

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 After the experiment is turned on, Initialize each peripheral first, then check if the font exists, If the detection is not problem, Start loop playbackSDCardMUSICarts
 *   Songs in the folder（Must be inSDCardCreate a root directoryMUSICartsParts，and store songs（Supported onlywavFormat）Inside），exist
 *   TFTLCDShow song name on、Play time、Total song time、Total number of songs、Information on the number of the current song。KEY0for selection
 *   A song，KEY2Used to select the previous song，KEY3Used to control pauses/Continue playing
 * 2 LED flashes, indicating that the program is running

 ***************************************************************************************************
 * Things to note
 * USART1The communication baud rate is115200
 * Please useXCOMSerial port debugging assistant，Other serial port software may controlDTR、RSTlead toMCUreset、The program does not run
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
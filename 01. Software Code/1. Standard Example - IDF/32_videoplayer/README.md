/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name: Video player Experiment
 * Experimental platform: ESP32-S3 development board
 * Purpose of experiment：studyMJPEGDecode andAVIFile format，implement a simpleAVIVideo player

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
 * 4 ALIENTEK 1.3/2.4inchSPILCDModule
 * 5 SD card, via SPI2 driver 
 * 6 ES8388AudioCODECchip
 * 7 MJPEGDecoding library
 * 8 Timer

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 After the experiment is turned on，Initialize the required peripherals first，然后检查Font library是否存在，If the font library does not exist，则更新Font library，if there is
 *   Font library，Start testingSDCard根目录下的VIDEOFolder videos（.aviFormat），If there is a video，Will play automatically。Play on video
 *   hour，LCDThe name of the video will be displayed、当前视频的编Number、Total number of videos、Number of channels、Audio sampling rate、Frame rate、播放hour间、总hour间
 *   Wait for information。KEY0Used to select next video，KEY2Used to select the previous video，KEY_UPFor fast forward，KEY1Used for quick retreat

 ***************************************************************************************************
 * Things to note
 * The communication baud rate of USART1 is 115200
 * Please useXCOMSerial port debugging assistant，Other serial port software may controlDTR、RSTlead toMCUReset、The program does not run
 * The IO4 pin of the development board is connected to RXD on the serial port to TTL module, and the IO5 pin is connected to TXD on the serial port to TTL module, and the module is then connected to the computer through a data cable.
 * Need to beSDThe card is correctly inserted into the onboardSDCard slot，Only then can this experimental routine be run normally

 ***********************************************************************************************************
 * Company Name:()
 * Phone number: 020-38271790
 * 传真Number码：020-36773971
 * Company website：
 * Purchase address: zhengdianyuanzi.tmall.com
 * Technical forum: http:///forum.php
 * Latest information：/docs/index.html

 * Online video：
 * B Site video：space.bilibili.com/394620890
 * male All Number：mp.weixin.qq.com/s/y--mG3qQT8gop0VRuER9bw
 * shake    sound：douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 ***********************************************************************************************************
 */
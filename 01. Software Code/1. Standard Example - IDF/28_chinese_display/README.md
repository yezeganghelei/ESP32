/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name: Chinese character display experiment
 * Experimental platform： ESP32-S3 Development Board
 * Purpose of experiment：Learn the principle of Chinese character display

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 1.3/2.4inchSPILCDModule
 * 2 SDCard,passSPI2drive
 * 3 Independent buttons
 *   KEY0(XL9555) - IO1_7
 * 4 XL9555
 *   IIC_SCL - IO42
 *   IIC_SDA - IO41
 * 5 LED
 *   LED - IO1

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 After the program runs，Will detect whether the font library exists in the partition table，If it does not exist, it willSDUpdate font library to partition table in card
 * 2 After the font library in the partition table is ready，Just start atLCDShow Chinese characters on it
 * 3 PressKEY0Press the key to force update the font library
 * 4 LEDFlashing，Indicates that the program is running

 ***************************************************************************************************
 * Things to note
 * none

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
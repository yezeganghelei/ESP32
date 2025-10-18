/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name: QMA6100P three-axis accelerometer sensor experiment
 * Experimental platform： ESP32-S3 Development Board
 * Purpose of experiment：studyQMA6100PThree-axis accelerometer and other measurements

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2 1.3/2.4inchSPILCDModule
 * 3 QMA6100P
 *   SDA - IO41
 *   CLK - IO42
 *   IO RXIO - P01

 ***************************************************************************************************
 * Experimental phenomenon
 * 1.This experiment,Test first when starting the machineQMA6100PDoes it exist，If not detectedQMA6100P，Then the error message is displayed on the monitor。If detectedQMA6100P，
 *   It will show normal，And in the main loop，Loop reading of three axesXYZRaw data and pitch angle、Rolling angle、Acceleration sensor data，
 *   And display inLCDOn the screen。
 * 2.LEDFlashing，Prompt the program to run。

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
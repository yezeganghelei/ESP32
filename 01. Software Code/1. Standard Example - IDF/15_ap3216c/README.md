/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name：Photoenvironment sensor experiment
 * Experimental platform： ESP32-S3 Development Board
 * Purpose of the experiment：Learning Light Environment SensorsAP3216CUse

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2  1.3/2.4inchSPILCDModule
 * 3 XL9535
 *   SCL - IO40
 *   SDA - IO41
 * 4 AP3216C
 *   SCL - IO40
 *   SDA - IO40

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 After the program is downloaded successfully, the LCD will display the ALS+PS+IR data of the light environment sensor. At the same time, the LED flashes, indicating that the system is running.
 * 2 Serial port printing experiment phenomenon.

 ***************************************************************************************************
 * Things to note
 * 1 ComputerSerial port debugging assistantThe baud rate must be115200
 * 2 Please useXCOM/SSCOMSerial port debugging assistant,Other serial assistants may controlDTR/RTSlead toMCUReset/The program does not run
 * 3 Serial printingExperimental phenomenon
 * 4 Please useUSBThe wire is connected toDevelopment BoardOnUART,And test this routine after connecting to the computer（Please select the correct port，Different devices will have different ports！）
 
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
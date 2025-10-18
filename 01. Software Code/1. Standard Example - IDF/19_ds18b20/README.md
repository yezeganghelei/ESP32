/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name：DS18B20Digital temperature sensorexperiment
 * Experimental platform： ESP32-S3 Development Board
 * Purpose of experiment：studyDS18B20Use of digital temperature sensors

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2  1.3/2.4inchSPILCDModule
 * 3 Digital temperature sensor
     DS18B20 - IO0

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 When this experiment is started, check whether there is anyDS18B20exist，If not，Then the prompt is an error。Only if detectedDS18B20It only started afterwards
     Read the temperature and display it inLCDsuperior，If foundDS18B20，Then every interval100msRead data left and right，and display the temperature inLCDsuperior。
 * 2 The LED flashes, prompting the program to run.

 ***************************************************************************************************
 * Things to note
 * Need to prepare your ownDS18B20Digital temperature sensorone，And insert it inDevelopment BoardofU4Location（Insert according to silk screen，Plane facing inward，Surface facing outward，Push it on the left）

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
 * shake    sound：douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 ***********************************************************************************************************
 */
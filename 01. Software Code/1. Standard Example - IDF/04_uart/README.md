/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name：Serial communication experiment
 * Experimental platform： ESP32-S3 Development Board
 * Purpose of experiment：studySerial portofuse(Receive and send)

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LEDlamp
     LED  -  IO1
 * 2 Serial port 0 (U0TX/U0RX is connected to the onboard USB to serial chip CH340)

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 LEDFlashing,Prompt the program is running。
 * 2 ESP32passSerial port0andsuperiorComputer dialogue,ESP32Received a string sent by the serial assistant(End with a carriage return)back,Will return toSerial portassistant。
     At the same time every certain time,passSerial port0Output a piece of information，可以passSerial portAssistant displays reception。

 ***************************************************************************************************
 * Things to note
 * 1 ComputerSerial portThe debugging assistant baud rate is adjusted to the same as this programof115200bps
 * 2 Please useXCOM/SSCOMSerial port调试assistant,otherSerial portAssistant may controlDTR/RTSlead toMCUReset/The program does not run
 * 3 Enter the string on the serial port and end with a carriage return line break
 * 4 Please useUSBThe wire is connected toUSB_UART,turn upUSBchangeSerial portbackTest this routine
 * 5 The U0TX/U0RX of the pin P4 must be connected to RXD/TXD through a jumper cap

 ***********************************************************************************************************
 * Company Name：（）
 * telephone number：020-38271790
 * Fax Number: 020-36773971
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
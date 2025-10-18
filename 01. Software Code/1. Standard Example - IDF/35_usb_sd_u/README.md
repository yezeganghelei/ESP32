/**
 ***************************************************************************************************
 * Introduction to the experiment
 * experiment名称：SDcard emulationUplate experiment
 * Experimental platform: ESP32-S3 development board
 * Experiment purpose：studyESP32-S3ofUSB HOSTFunction usage,Realize the rightUplateof读写

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2 XL9555
     IIC_SCL - IO42
     IIC_SDA - IO41
 * 3 1.3/2.4inchSPILCDModule

 ***************************************************************************************************
 *Experimental phenomena
 * 1 USBLine insertionESP32-S3Development boardUSBAfter the port，System handleSDCard simulationUplate，You can see it on your computerUplateof磁plate显示。
 * 2 LED flashes, indicating that the program is running

 ***************************************************************************************************
 * Things to note
 * USART1of通讯波特率for115200
 * Please useXCOMSerial port debugging assistant，Other serial port software may controlDTR、RSTlead toMCUReset、The program does not run
 * Openmenuconfigmenu，turn upMassive Storage ClassOptions，Must be turned onTinyUSB MSC feature and setMSC FIFO sizefor4096(4K)

 ***********************************************************************************************************
 * Company Name:()
 * Telephone number: 020-38271790
 * 传真Number码：020-36773971
 * Company website：
 * Purchase address: zhengdianyuanzi.tmall.com
 * Technical forum: http:///forum.php
 * Latest information：/docs/index.html

 * online video：
 * B Site video：space.bilibili.com/394620890
 * male All Number：mp.weixin.qq.com/s/y--mG3qQT8gop0VRuER9bw
 * shake    sound：douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 ***********************************************************************************************************
 */
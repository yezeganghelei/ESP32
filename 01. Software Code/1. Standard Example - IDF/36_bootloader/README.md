/**
 ***************************************************************************************************
 * Introduction to the experiment
 * experiment名称：FLASHsimulationUplate experiment
 * Experimental platform: ESP32-S3 development board
 * Experiment purpose：studyESP32-S3ofUSB MSCFunction usage,Realize the rightFLASHsimulationUplateof读写

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
 * 1 USBLine insertionESP32-S3Development BoardUSBafter port，Systemstorage分区simulation成Uplate，You can see it on your computerUplateof磁plate显示。
 * 2 LED flashes, indicating that the program is running

 ***************************************************************************************************
 * Things to note
 * USART1of通讯波特率for115200
 * Please useXCOMSerial port debugging assistant，Other serial port software may controlDTR、RSTlead toMCUReset、The program does not run
 * Openmenuconfigmenu，turn upMassive Storage ClassOptions，Must be turned onTinyUSB MSC feature And setMSC FIFO sizefor4096(4K)

 ***********************************************************************************************************
 * Company Name:()
 * Phone number: 020-38271790
 * 传真Number码：020-36773971
 * Company website：
 * Purchase address: zhengdianyuanzi.tmall.com
 *Technical forum: http:///forum.php
 * Latest information：/docs/index.html

 * online video：
 * B site video：space.bilibili.com/394620890
 * male public Number：mp.weixin.qq.com/s/y--mG3qQT8gop0VRuER9bw
 * shake    sound：douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 ***********************************************************************************************************
 */
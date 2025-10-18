/**
 ***************************************************************************************************
 * Introduction to the experiment
 * Experiment name: Watchdog experiment
 * Experimental platform： ESP32-S3 Development Board
 * Purpose of the experiment：studyWatchdogUse(Timer simulationWatchdogFunction)

 ***************************************************************************************************
 * Hardware resources and pin allocation
 * 1 LED
     LED - IO1
 * 2 Watchdog (using ESP timer for simulation)

 ***************************************************************************************************
 * Experimental phenomenon
 * 1 After the program runs，Development BoardOnboardLEDFlashes due to constant reset，But if pressBOOTPress the button to feed the dog，
     Then there will be no reset，LEDKeep it always bright，Once the timer overflow time has exceeded（timeout_us = 1000000subtle），
     If the BOOT button is not pressed to feed the dog, it will be reset and the LED will be turned off once.
     Use this function during dog feeding：esp_err_t esp_timer_restart(esp_timer_handle_t timer, uint64_t timeout_us)

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
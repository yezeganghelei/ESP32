# take_pic_from_mqtt

# Table of contents

- [1.Function description](#Funtion)  
- [2.Prepare](#hardwareprepare)  
- [3.serverPrepare](#clouds)  
- [4.Detailed explanation of embedded code](#device)  
- [5. List of recommended open source and fun DIYs](#other)  

## <span id = "Funtion">one、Function description</span>

This article is based on the Linux environment, secondary development through Essinco ESP32-Cam development board SDK C language programming, and the video stream of the camera can be viewed through the local LAN.

## <span id = "Introduction">two、Prepare</span>

## ## hardware

- Anxinke ESP32-Cam development board: https://item.taobao.com/item.htm?id=573698917181

- TTL-USB Debugging tools（Recommended to use this，Ensure sufficient voltage and current）：https://item.taobao.com/item.htm?id=565546260974

### software

- Environment setup: [Linux environment setup /relese/v3.3.2 branch](https://docs.espressif.com/projects/esp-idf/zh_CN/v3.3.2/get-started/linux-setup.html)
- Toolchain settings：download toolchain，The version used by the blogger is：```gcc version 5.2.0 (crosstool-NG crosstool-ng-1.22.0-97-gc752ad5)```
- Python version: `Python 2.7.17`

## <span id = "clouds">three、Set up routerSSIDand password

exist `app_wifi.c`Router account password of the file，Its format is as follows：

```c
void app_wifi_initialise(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "aiot@xuhongv",
            .password = "12345678",
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}
```

## <span id = "device">Four、Log printing

File directory description：

```
├─1_take_pic_http_to_cloud 
│ ├─components Camera driver code component
│ ├─main user program
│ │ ├─app_wifi.c/h Logic implementation for connecting to routers
│ │ ├─main.c Main file program entry
```

LOGLog printing，可以看到exist本地局域网的IPThe address is：`10.10.10.114`，The access address is：`10.10.10.114:80/stream`

```c
I (479) gpio: GPIO[32]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (629) camera: Detected OV2640 camera
I (639) camera: Allocating 1 frame buffers (60 KB total)
I (639) camera: Allocating 60 KB frame buffer in OnBoard RAM
I (1479) wifi:new:<9,0>, old:<1,0>, ap:<255,255>, sta:<9,0>, prof:1
I (2469) wifi:state: init -> auth (b0)
I (2479) wifi:state: auth -> assoc (0)
I (2479) wifi:state: assoc -> run (10)
I (2769) wifi:connected with aiot@xuhongv, aid = 1, channel 9, BW20, bssid = d8:c8:e9:05:c4:d8
I (2769) wifi:security type: 4, phy: bgn, rssi: -37
I (2769) wifi:pm start, type: 1

I (2839) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (3699) event: sta ip: 10.10.10.114, mask: 255.255.255.0, gw: 10.10.10.1
I (3699) example:take_picture: Http Start
I (3699) example:take_picture: Starting server on port: '80'
I (3699) example:take_picture: Registering URI handlers
I (3709) example:take_picture: Http End
```

## <span id = "device">five、Recommend open source and funDIY的one览表

| Open source projects                                                 | address                                                        | Open source time   |
| -------------------------------------------------------- | ----------------------------------------------------------- | ---------- |
| ESP32-Cam摄像头拍照上传到私有server                      |                                                             | 2020.12.30 |
| WeChat mini program connectionmqttserver，controlesp8266Smart hardware            | https://github.com/xuhongv/WeChatMiniEsp8266                | 2018.11    |
| WeChat public accountairkissDistribution network以及近场发现existesp8266Realization         | https://github.com/xuhongv/xLibEsp8266Rtos3.1AirKiss        | 2019.3     |
| WeChat public accountairkissDistribution network以及近场发现existesp32/esp32S2Realization   | https://github.com/xuhongv/xLibEsp32IdfAirKiss              | 2019.9     |
| WeChat appletcontrolesp8266实现七彩效果项目Source code                | https://github.com/xuhongv/WCMiniColorSetForEsp8266         | 2019.9     |
| WeChat applet Bluetooth configuration networkblufi实现existesp32Source code                   | https://github.com/xuhongv/BlufiEsp32WeChat                 | 2019.11    |
| WeChat applet Bluetooth ble controls esp32 colorful light effect | https://blog.csdn.net/xh870189248/article/details/101849759 | 2019.10 |
| 可商用的事件分发的WeChat appletmqttDisconnection and reconnection framework             | https://blog.csdn.net/xh870189248/article/details/88718302  | 2019.2     |
| WeChat applet以 websocket Connect to Alibaba CloudIOTIoT platformmqttserver | https://blog.csdn.net/xh870189248/article/details/91490697  | 2019.6     |
| WeChat public account网页实现连接mqttserver                         | https://blog.csdn.net/xh870189248/article/details/100738444 | 2019.9     |
| WeChat appletAPDistribution networkWi-Fi ESP32-S2module                       | https://github.com/xuhongv/WeChatMiniAP2Net                 | 2020.9.21  |
| 云云对接方案serverSource code xClouds-php PHP TP5development framework       | https://github.com/xuhongv/xClouds-php                      | 2020.8.4   |
| WeChat applet端自定义viewColorful collection color ring control               | https://github.com/xuhongv/WCMiniColorPicker                | 2019.12.04 |
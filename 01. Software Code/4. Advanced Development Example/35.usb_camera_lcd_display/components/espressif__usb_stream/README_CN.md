[![Component Registry](https://components.espressif.com/components/espressif/usb_stream/badge.svg)](https://components.espressif.com/components/espressif/usb_stream)

## # USB Stream component description

``usb_stream`` is a USB UVC + UAC host driver based on ESP32-S2/ESP32-S3, which supports reading/writing/controlling multimedia streams from USB devices. For example, it supports up to 1 camera + 1 microphone + 1 player data stream at the same time.

characteristic:

1. Supports obtaining video streams through the UVC Stream interface, and supports two transmission modes: batch and synchronous
2. Support through UAC Stream Interface to obtain microphone data stream，Send player data stream
3. Support through UAC Control Interface controls microphone volume、Silent features
4. Supports pausing and resuming data streams

### USB Stream User Guide

Please refer to：https://docs.espressif.com/projects/esp-iot-solution/zh_CN/latest/usb/usb_stream.html

## ## Add components to the project

Please use the component manager directive `add-dependency` Will `usb_stream` 添加到项目of依赖项, exist `CMake` The component will be automatically downloaded to the project directory during execution.。

```
idf.py add-dependency "espressif/usb_stream=*"
```

## ## Sample program

Please use the component manager directive `create-project-from-example` Download the default sample program for this component in the current folder。

* USB Camera WIFI image transmission
```
idf.py create-project-from-example "espressif/usb_stream=*:usb_camera_mic_spk"
```

* USB Camera Local refresh
```
idf.py create-project-from-example "espressif/usb_stream=*:usb_camera_lcd_display"
```

After entering the sample program directory, you can proceed to the next step of compilation and burning.

> You can also choose from esp-iot-solution Download the sample program directly from the warehouse: [USB Camera + Audio stream](https://github.com/espressif/esp-iot-solution/tree/master/examples/usb/host/usb_camera_mic_spk), [USB Camera LCD Display](https://github.com/espressif/esp-iot-solution/tree/master/examples/usb/host/usb_camera_lcd_display).

## ## Q&A

Q1. I encountered the following problems when adding components using the package manager

```
Executing action: create-project-from-example
CMakeLists.txt not found in project directory /home/username
```

A1. 这是因为您使用了一个老of组件管理器版本, 请exist ESP-IDF Run in the environment `pip install -U idf-component-manager` Update component。
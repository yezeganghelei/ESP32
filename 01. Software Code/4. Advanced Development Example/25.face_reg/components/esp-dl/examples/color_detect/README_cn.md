# Color detection [[English]](./README.md)

This project is an example of a color detection interface. The input image of the color detection interface is a static image with different color patches.
The output is the input color、Detect color、split color， 删除颜色等接口功能的run结果，displayed in terminal。

The folder structure of the project is as follows：

```shell
color_detect/
├── CMakeLists.txt
├── rgby.jpg
├── main
│   ├── app_main.cpp
│   ├── CMakeLists.txt
│   └── image.hpp
├── partitions.csv
└── README.md
└── README_cn.md
```

## Run the example

1. Open terminal，进入Color detection示例所在文件夹 esp-dl/examples/color_detect：

    ```shell
    cd ~/esp-dl/examples/color_detect
    ```

2. Set the target chip:

    ```shell
    idf.py set-target [SoC]
    ```
    Will [SoC] 替换为您的目标chip，like esp32、esp32s2、esp32s3。
    
    Since the ESP32-S3 chip runs much faster than other chips for AI applications, we recommend you use the ESP32-S3 chip.

3. Burning program，run IDF The monitor obtains the running results of each function：

   ```shell
   idf.py flash monitor
   
   ... ...
   
   the information of registered colors: 
   name: red, 	thresh: 0, 10, 203, 255, 197, 255
   name: green, 	thresh: 54, 62, 221, 255, 197, 255
   name: blue, 	thresh: 96, 114, 179, 255, 230, 255
   name: yellow, 	thresh: 19, 32, 214, 255, 247, 255
   
   RGB888 | color detection result:
   color 0: detected box :2
   center: (46, 14)
   box: (0, 0), (94, 30)
   area: 768
   center: (14, 110)
   box: (0, 96), (30, 126)
   area: 256
   
   color 1: detected box :2
   center: (110, 30)
   box: (96, 0), (126, 62)
   area: 512
   center: (30, 46)
   box: (0, 32), (62, 62)
   area: 512
   
   color 2: detected box :2
   center: (88, 68)
   box: (64, 32), (126, 94)
   area: 768
   center: (14, 78)
   box: (0, 64), (30, 94)
   area: 256
   
   color 3: detected box :1
   center: (70, 102)
   box: (32, 64), (126, 126)
   area: 1024

   RGB565 | color detection result:
   color 0: detected box :2
   center: (46, 14)
   box: (0, 0), (94, 30)
   area: 768
   center: (14, 110)
   box: (0, 96), (30, 126)
   area: 256
   
   color 1: detected box :2
   center: (110, 30)
   box: (96, 0), (126, 62)
   area: 512
   center: (30, 46)
   box: (0, 32), (62, 62)
   area: 512
   
   color 2: detected box :2
   center: (88, 68)
   box: (64, 32), (126, 94)
   area: 768
   center: (14, 78)
   box: (0, 64), (30, 94)
   area: 256
   
   color 3: detected box :1
   center: (70, 102)
   box: (32, 64), (126, 126)
   area: 1024
   
   remained colors num: 3
   
   Blue, Yellow | color detection result:
   color 0: detected box :2
   center: (88, 68)
   box: (64, 32), (126, 94)
   area: 768
   center: (14, 78)
   box: (0, 64), (30, 94)
   area: 256
   
   color 1: detected box :1
   center: (70, 102)
   box: (32, 64), (126, 126)
   area: 1024

   Blue, Yellow | color segmentation result:
   color 0: detected box :2
   box_index: 0, start col: 32, end col: 47, row: 16, area: 768
   box_index: 0, start col: 32, end col: 47, row: 17, area: 768
   box_index: 0, start col: 32, end col: 47, row: 18, area: 768
   box_index: 0, start col: 32, end col: 47, row: 19, area: 768
   box_index: 0, start col: 32, end col: 47, row: 20, area: 768
   box_index: 0, start col: 32, end col: 47, row: 21, area: 768
   box_index: 0, start col: 32, end col: 47, row: 22, area: 768
   box_index: 0, start col: 32, end col: 47, row: 23, area: 768
   box_index: 0, start col: 32, end col: 47, row: 24, area: 768
   box_index: 0, start col: 32, end col: 47, row: 25, area: 768
   box_index: 0, start col: 32, end col: 47, row: 26, area: 768
   box_index: 0, start col: 32, end col: 47, row: 27, area: 768
   box_index: 0, start col: 32, end col: 47, row: 28, area: 768
   box_index: 0, start col: 32, end col: 47, row: 29, area: 768
   box_index: 0, start col: 32, end col: 47, row: 30, area: 768
   box_index: 0, start col: 32, end col: 47, row: 31, area: 768
   box_index: 1, start col: 0, end col: 15, row: 32, area: 256
   box_index: 0, start col: 32, end col: 63, row: 32, area: 768
   box_index: 1, start col: 0, end col: 15, row: 33, area: 256
   box_index: 0, start col: 32, end col: 63, row: 33, area: 768
   box_index: 1, start col: 0, end col: 15, row: 34, area: 256
   box_index: 0, start col: 32, end col: 63, row: 34, area: 768
   box_index: 1, start col: 0, end col: 15, row: 35, area: 256
   box_index: 0, start col: 32, end col: 63, row: 35, area: 768
   box_index: 1, start col: 0, end col: 15, row: 36, area: 256
   box_index: 0, start col: 32, end col: 63, row: 36, area: 768
   box_index: 1, start col: 0, end col: 15, row: 37, area: 256
   box_index: 0, start col: 32, end col: 63, row: 37, area: 768
   box_index: 1, start col: 0, end col: 15, row: 38, area: 256
   box_index: 0, start col: 32, end col: 63, row: 38, area: 768
   box_index: 1, start col: 0, end col: 15, row: 39, area: 256
   box_index: 0, start col: 32, end col: 63, row: 39, area: 768
   box_index: 1, start col: 0, end col: 15, row: 40, area: 256
   box_index: 0, start col: 32, end col: 63, row: 40, area: 768
   box_index: 1, start col: 0, end col: 15, row: 41, area: 256
   box_index: 0, start col: 32, end col: 63, row: 41, area: 768
   box_index: 1, start col: 0, end col: 15, row: 42, area: 256
   box_index: 0, start col: 32, end col: 63, row: 42, area: 768
   box_index: 1, start col: 0, end col: 15, row: 43, area: 256
   box_index: 0, start col: 32, end col: 63, row: 43, area: 768
   box_index: 1, start col: 0, end col: 15, row: 44, area: 256
   box_index: 0, start col: 32, end col: 63, row: 44, area: 768
   box_index: 1, start col: 0, end col: 15, row: 45, area: 256
   box_index: 0, start col: 32, end col: 63, row: 45, area: 768
   box_index: 1, start col: 0, end col: 15, row: 46, area: 256
   box_index: 0, start col: 32, end col: 63, row: 46, area: 768
   box_index: 1, start col: 0, end col: 15, row: 47, area: 256
   box_index: 0, start col: 32, end col: 63, row: 47, area: 768
   
   color 1: detected box :1
   box_index: 0, start col: 16, end col: 31, row: 32, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 33, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 34, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 35, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 36, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 37, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 38, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 39, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 40, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 41, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 42, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 43, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 44, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 45, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 46, area: 1024
   box_index: 0, start col: 16, end col: 31, row: 47, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 48, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 49, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 50, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 51, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 52, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 53, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 54, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 55, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 56, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 57, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 58, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 59, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 60, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 61, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 62, area: 1024
   box_index: 0, start col: 16, end col: 63, row: 63, area: 1024
   
   ```
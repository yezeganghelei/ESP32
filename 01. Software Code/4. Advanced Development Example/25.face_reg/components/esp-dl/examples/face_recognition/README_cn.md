# Face recognition [[English]](./README.md)

This project is an example of a face recognition interface. The input picture of the face recognition interface is a static picture with a face, and the output is the running result of the interface functions such as face entry, face recognition, and face deletion, which is displayed in the terminal.

This interface provides 16 bit quantization vs. 8 Bit-quantized two versions of the model。16 The bit-quantized model is compared to 8 bit quantized model，Higher accuracy，But it takes up more memory，Also runs slower。您可以根据实际use场景挑选合适的模型。

The folder structure of the project is as follows：

```shell
face_recognition/
├── CMakeLists.txt
├── image.jpg
├── main
│   ├── app_main.cpp
│   ├── CMakeLists.txt
│   └── image.hpp
├── partitions.csv
└── README.md
└── README_cn.md
```

## Run the example

1. Open terminal，Enter the folder where the face detection example is located esp-dl/examples/face_recognition

    ```shell
    cd ~/esp-dl/examples/face_recognition
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
   
   E (1907) MFN: Flash is empty
   
   enroll id ...
   name: Sandra, id: 1
   name: Jiong, id: 2
   
   recognize face ...
   [recognition result] id: 1, name: Sandra, similarity: 0.728666
   [recognition result] id: 2, name: Jiong, similarity: 0.827225
   
   recognizer information ...
   recognizer threshold: 0.55
   input shape: 112, 112, 3
   
   face id information ...
   number of enrolled ids: 2
   id: 1, name: Sandra
   id: 2, name: Jiong
   
   delete id ...
   number of remaining ids: 1
   [recognition result] id: -1, name: unknown, similarity: 0.124767
   
   enroll id ...
   name: Jiong, id: 2
   write 2 ids to flash.
   
   recognize face ...
   [recognition result] id: 1, name: Sandra, similarity: 0.758815
   [recognition result] id: 2, name: Jiong, similarity: 0.722041
   
   ```

## # Other settings

1. [./main/app_main.cpp](./main/app_main.cpp) Macro definition at the beginning `QUANT_TYPE`，Definable quantification type of the model。

    - `QUANT_TYPE` = 0：use 8 bit quantization model，Recognition accuracy is lower than 16 bit model，but faster，Less memory usage。
    - `QUANT_TYPE` = 1: Use 16-bit quantization model, and the recognition accuracy is consistent with the floating point model.

    You can choose the appropriate model according to the actual usage scenario。

2. [./main/app_main.cpp](./main/app_main.cpp) Macro definition at the beginning `USE_FACE_DETECTOR`，Definable facial key points (landmark) How to obtain coordinates。

    - `USE_FACE_DETECTOR` = 0：use存放在 ./image.hpp key point coordinates in。
    - `USE_FACE_DETECTOR` = 1：Use face detection model to obtain key point coordinates。

   Please note that the key point coordinate sequence is：
   
   ```
    left_eye_x, left_eye_y, 
    mouth_left_x, mouth_left_y,
    nose_x, nose_y,
    right_eye_x, right_eye_y, 
    mouth_right_x, mouth_right_y
   ```

## Delay

|   SoC    |      8 Bit |    16 Bit |
| :------: | --------: | -------: |
|  ESP32   | 13,301 ms | 5,041 ms |
| ESP32-S3 |    287 ms |   554 ms |
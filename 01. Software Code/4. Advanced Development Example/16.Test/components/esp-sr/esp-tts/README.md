# ESP Chinese TTS [[English]](./README_en.md) 

乐鑫middle文语音合成是一个为嵌入式系统设计的轻量化语音合成系统。

## Overview

The current version of Espressif Systems speech synthesis is based on the splicing method. The system block diagram is as follows:

![chinese TTS](./img/esp_chinese_tts.png)

- Parser: According to dictionary and grammar rules，Convert input text to pinyin list, The input text is encoded asUTF-8。
- Synthesizer: according toParserOutput pinyin list，Combine predefined sound sets，Synthesized wave file。The default output format is mono， 16bit@16000Hz。

#### Features：

- [x] UTF-8 encoded input

- [x] Streaming output，Reduce latency

- [x] Automatic recognition of polysyllabic word pronunciation

- [x] Adjustable synthesized speech speed

- [x] Digital broadcast optimization

- [ ] Custom sound set

## Performance Test

#### Resource Occupancy

Flash image size： 2.2 MB

RAM runtime: 20 KB

CPU loading test (based on ESP32 @ 240MHz test):

| speech rate                 |  0   |  1   |  2   |  3   |  4   |  5   |
| --------------------------- | :--: | :--: | :--: | :--: | :--: | :--: |
| times faster than real time | 4.5  | 3.2  |  2.9 | 2.5  | 2.2  | 1.8  |

#### Samples

- Welcome to Espressif Systems Speech Synthesis, &nbsp; &nbsp; [voice=Xiaole,speed=0](./samples/S1_xiaole_speed0.wav), &nbsp; &nbsp;  [voice=Xiaole,speed=2](./samples/S1_xiaole_speed2.wav) 

- Alipay payment 1111.11 Yuan, &nbsp; &nbsp;  [voice=Xiaole,speed=0](./samples/S1_xiaole_speed0.wav), &nbsp; &nbsp;  [voice=Xiaole,speed=2](./samples/S2_xiaole_speed2.wav) 

- Air conditioning heating mode is on，and adjust to25Spend, &nbsp; &nbsp;  [voice=Xiaole,speed=0](./samples/S3_xiaole_speed0.wav), &nbsp; &nbsp;   [voice=Xiaole,speed=4](./samples/S3_xiaole_speed4.wav) 

## User Guide

```c
#include "esp_tts.h"
#include "esp_tts_voice_female.h"
#include "esp_partition.h"

/*** 1. create esp tts handle  ***/

//// Method1: use pre-define xiaole voice lib.
//// This method is not recommended because the method may make app bin exceed the limit of esp32
// esp_tts_handle_t *tts_handle=esp_tts_create(esp_tts_voice_female);

// method2: initial voice set from separate voice data partition

const esp_partition_t* part=esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "voice_data");
if (part==0) printf("Couldn't find voice data partition!\n");
spi_flash_mmap_handle_t mmap;
uint16_t* voicedata;
esp_err_t err=esp_partition_mmap(part, 0, 3*1024*1024, SPI_FLASH_MMAP_DATA, (const void**)&voicedata, &mmap);
esp_tts_voice_t *voice=esp_tts_voice_set_init(&esp_tts_voice_template, voicedata); 

// 2. parse text and synthesis wave data
char *text="Welcome to Espressif Systems Speech Synthesis";	
if (esp_tts_parse_chinese(tts_handle, text)) {  // parse text into pinyin list
	int len[1]={0};
	do {
		short *data=esp_tts_stream_play(tts_handle, len, 4); // streaming synthesis
		i2s_audio_play(data, len[0]*2, portMAX_DELAY);  // i2s output             
	} while(len[0]>0);
	i2s_zero_dma_buffer(0);
}

```

For more please refer to[esp_tts.h](./esp_tts_chinese/include/esp_tts.h)CheckAPIdefinition, or referenceesp-skainetmiddle[chinese_tts](../../examples/chinese_tts)Example.
/**
 ****************************************************************************************************
 * @file        recorder.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       wavdecoding Code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "recorder.h"

uint32_t g_wav_size;    /* wavdata大小(Number of bytes,不包括文件head!!) */

uint8_t g_rec_sta = 0;  /**
                         * recording状态
                         * [7]:0,没有Openrecording;1,已经Openrecording;
                         * [6:1]:reserve
                         * [0]:0,正在recording;1,pauserecording;
                         */

/**
 * @brief       EnterPCM recording模式
 * @param       none
 * @retval      none
 */
void recoder_enter_rec_mode(void)
{
    es8388_adda_cfg(0, 1);          /* Turn on the ADC */
    es8388_input_cfg(0);            /* Open输入aisle(aisle1,MICChannel) */
    es8388_mic_gain(8);             /* MICGain set to maximum */
    es8388_alc_ctrl(3, 4, 4);       /* Open立体声ALCcontrol,以提高recording音量 */
    es8388_output_cfg(0, 0);        /* Close the outputs of channels 1 and 2 */
    es8388_spkvol_set(0);           /* Turn off the speaker. */
    es8388_sai_cfg(0, 3);           /* Philips standard, 16-bit data length */
    i2s_set_samplerate_bits_sample(SAMPLE_RATE,I2S_BITS_PER_SAMPLE_16BIT);    /* initializationI2S */
    i2s_trx_start();                /* Turn on I2S */
    recoder_remindmsg_show(0);
}

/**
 * @brief       EnterPCM Playback mode
 * @param       none
 * @retval      none
 */
void recoder_enter_play_mode(void)
{
    es8388_adda_cfg(1, 0);      /* OpenDAC */
    es8388_output_cfg(1, 1);    /* Openaisle1and2Output */
    es8388_spkvol_set(28);      /* Speaker volume settings */
    i2s_trx_stop();             /* 停止recording */
    recoder_remindmsg_show(1);  /* Show prompt information */
}

/**
 * @brief       initializationWAVhead
 * @param       wavhead : wav文件head指针
 * @retval      none
 */
void recoder_wav_init(__WaveHeader *wavhead)
{
    wavhead->riff.ChunkID = 0x46464952;                  /* RIFF" */
    wavhead->riff.ChunkSize = 0;                         /* Not confirmed yet,Finally, it needs to be calculated */
    wavhead->riff.Format = 0x45564157;                   /* "WAVE" */
    wavhead->fmt.ChunkID = 0x20746D66;                   /* "fmt " */
    wavhead->fmt.ChunkSize = 16;                         /* Size is16Bytes */
    wavhead->fmt.AudioFormat = 0x01;                     /* 0x01 means PCM; 0x00 means IMA ADPCM */
    wavhead->fmt.NumOfChannels = 2;                      /* Two-channel */
    wavhead->fmt.SampleRate = SAMPLE_RATE;               /* Sampling rate */
    wavhead->fmt.ByteRate = wavhead->fmt.SampleRate * 4; /* byte rate=Sampling rate*aisle数*(ADCBit数/8) */
    wavhead->fmt.BlockAlign = 4;                         /* Block size = number of channels*(number of ADC bits/8) */
    wavhead->fmt.BitsPerSample = 16;                     /* 16BitPCM */
    wavhead->data.ChunkID = 0x61746164;                  /* "data" */
    wavhead->data.ChunkSize = 0;                         /* data大小,Requires calculations */
}

/**
 * @brief       显示recordingtimeandCode rate
 * @param       tsec : time（unit : Second）
 * @param       kbps : Code rate
 * @retval      none
 */
void recoder_msg_show(uint32_t tsec, uint32_t kbps)
{
    lcd_show_string(30, 210, 200, 16, 16, "TIME:", RED);    /* 显示recordingtime */
    lcd_show_num(30 + 40, 210, tsec / 60, 2, 16, RED);      /* minute */
    lcd_show_char(30 + 56, 210, ':', 16, 0, RED);
    lcd_show_num(30 + 64, 210, tsec % 60, 2, 16, RED);      /* Seconds */

    lcd_show_string(140, 210, 200, 16, 16, "KPBS:", RED);   /* Display code rate */
    lcd_show_num(140 + 40, 210, kbps / 1000, 4, 16, RED);   /* Bit rate display */
}

/**
 * @brief       Prompt information
 * @param       mode : Working mode
 *   @arg       0, recording模式
 *   @arg       1, Playback mode
 * @retval      none
 */
void recoder_remindmsg_show(uint8_t mode)
{
    lcd_fill(30, 120, lcd_self.width, 180, WHITE);            /* Clear the original display */

    if (mode == 0)  /* recording模式 */
    {
        lcd_show_string(30, 120, 200, 16, 16, "KEY0:REC/PAUSE", BLUE);
        lcd_show_string(30, 140, 200, 16, 16, "KEY2:STOP&SAVE", BLUE);
        lcd_show_string(30, 160, 200, 16, 16, "KEY3:PLAY", BLUE);
    }
    else            /* Playback mode */
    {
        lcd_show_string(30, 120, 200, 16, 16, "KEY0:STOP Play", BLUE);
        lcd_show_string(30, 140, 200, 16, 16, "KEY3:PLAY/PAUSE", BLUE);
    }
}

/**
 * @brief       Get file name through time
 * @note        Only inSDcard save,Not supportedFLASH DISKsave
 * @note        Combined:Like"0:RECORDER/REC00001.wav"file name
 * @param       pname : File path
 * @retval      none
 */
void recoder_new_pathname(uint8_t *pname)
{
    uint8_t res;
    uint16_t index = 0;
    FIL *ftemp;
    ftemp = (FIL *)malloc(sizeof(FIL));           /* open upFILBytes memory area */

    if (ftemp == NULL) 
    {
        return;  /* Memory application failed */
    }

    while (index < 0xFFFF)
    {
        sprintf((char *)pname, "0:RECORDER/REC%05d.wav", index);
        res = f_open(ftemp, (const TCHAR *)pname, FA_READ); /* Try to open this file */

        if (res == FR_NO_FILE)
        {
            break;              /* The file name does not exist=It's exactly what we need. */
        }

        index++;
    }

    free(ftemp);
}

/**
 * @brief WAV recording
 * @param None
 * @retval None
 */
void wav_recorder(void)
{
    uint8_t res;
    uint8_t key;
    uint8_t rval = 0;
    uint32_t bw;
    
    __WaveHeader *wavhead = 0;
    FF_DIR recdir;          /* Table of contents */
    FIL *f_rec;             /* recording文件 */
    
    uint8_t *pdatabuf;      /* Data cache pointer */
    uint8_t *pname = 0;     /* File name */
    uint32_t recsec = 0;    /* recordingtime */
    uint8_t timecnt = 0;    /* Timer */
    uint16_t bytes_read = 0;

    while (f_opendir(&recdir, "0:/RECORDER"))   /* 打开recording文件夹 */
    {
        lcd_show_string(30, 230, 240, 16, 16, "RECORDER folder error!", RED);
        vTaskDelay(200);
        lcd_fill(30, 230, 240, 246, WHITE);     /* Clear the display */
        vTaskDelay(200);
        f_mkdir("0:/RECORDER");                 /* 创建该Table of contents */
    }

    pdatabuf = malloc(1024 * 10);                           /* recording存储区 */
    f_rec = (FIL*)malloc(sizeof(FIL));                      /* open upFILBytes memory area */
    wavhead = (__WaveHeader *)malloc(sizeof(__WaveHeader)); /* open up__WaveHeaderBytes memory area */
    pname = malloc(30);   /* Apply30Bytes内存,File name similar"0:RECORDER/REC00001.wav" */

    if (!f_rec || !wavhead || !pname || !pdatabuf)
    {
        rval = 1;           /* If any one of them fails, it fails */
    }

    if (rval == 0)
    {
        recoder_enter_rec_mode();   /* Enterrecording模式,此时耳机可以听到咪head采集到的音频 */
        pname[0] = 0;               /* pname does not have any file name */

        while (rval == 0)
        {
            key = xl9555_key_scan(0);

            switch (key)
            {
                case KEY2_PRES:                                     /* STOP&SAVE */
                    if (g_rec_sta & 0x80)                           /* 有recording */
                    {
                        g_rec_sta = 0;                              /* closurerecording */
                        wavhead->riff.ChunkSize = g_wav_size + 36;  /* The size of the entire file-8; */
                        wavhead->data.ChunkSize = g_wav_size;       /* data大小 */
                        f_lseek(f_rec, 0);                          /* 偏移到文件head. */
                        f_write(f_rec, (const void *)wavhead, sizeof(__WaveHeader), &bw); /* 写入headdata */
                        f_close(f_rec);
                        g_wav_size = 0;
                    }

                    g_rec_sta = 0;
                    recsec = 0;
                    LED(1);        /* closureDS0 */
                    lcd_fill(30, 190, lcd_self.width, lcd_self.height, WHITE); /* Clear the display,清除之前显示的recording文件名 */
                    break;
 
                case KEY0_PRES:     /* REC/PAUSE */
                    if (g_rec_sta & 0x01)                           /* If it is pause,继续recording */
                    {
                        g_rec_sta &= 0xFE;                          /* Cancel the pause */
                    }
                    else if (g_rec_sta & 0x80)                      /* 已经在recording了,pause */
                    {
                        g_rec_sta |= 0x01;                          /* pause */
                    }
                    else                                            /* 还没开始recording */
                    {
                        recsec = 0;
                        recoder_new_pathname(pname);                /* Get a new name */
                        text_show_string(30, 190, lcd_self.width, 16, "Recording:", 16, 0, RED);
                        text_show_string(30 + 40, 190, lcd_self.width, 16, (char *)pname + 11, 16, 0, RED);   /* 显示当前recording文件名字 */
                        recoder_wav_init(wavhead);                  /* initializationwavdata */
                        res = f_open(f_rec, (const TCHAR*)pname, FA_CREATE_ALWAYS | FA_WRITE);    /* Open the file */

                        if (res)                        /* File creation failed */
                        {
                            g_rec_sta = 0;              /* Failed to create a file,不能recording */
                            rval = 0xFE;                /* Tip if the SD card exists */
                        }
                        else
                        {
                            res = f_write(f_rec, (const void *)wavhead, sizeof(__WaveHeader), (UINT*)&bw); /* 写入headdata */
                            recoder_msg_show(0, 0);
                            g_rec_sta |= 0x80;          /* 开始recording */
                        }
                    }

                    if (g_rec_sta & 0x01)
                    {
                        LED(0);                         /* 提示正在pause */
                    }
                    else 
                    {
                        LED(1);
                    }
                    break;

                case KEY3_PRES:                         /* Play最近一段recording */
                    if (g_rec_sta != 0x80)              /* 没有在recording */
                    {
                        if (pname[0])                   /* If the key is pressed and the pname is not empty */
                        {
                            text_show_string(30, 190, lcd_self.width, 16, "Play:", 16, 0, RED);
                            text_show_string(30 + 40, 190, lcd_self.width, 16, (char *)pname + 11, 16, 0, RED); /* Show the file name when playing */
                            recoder_enter_play_mode();  /* Enter play mode */
                            audio_play_song(pname);     /* Play pname */
                            lcd_fill(30, 190, lcd_self.width, lcd_self.height, WHITE); /* Clear the display,清除之前显示的recording文件名 */
                            recoder_enter_rec_mode();   /* 重新Enterrecording模式 */
                        }
                    }
                    break;
            }

            if ((g_rec_sta & 0x80) == 0x80)
            {
                if ((g_rec_sta & 0x01) == 0x00)
                {
                    bytes_read = i2s_rx_read((uint8_t *)pdatabuf, 1024 * 10);
                    res = f_write(f_rec, pdatabuf, bytes_read, (UINT*)&bw);  /* Write to a file */

                    if (res)
                    {
                        printf("write error:%d\r\n", res);
                    }

                    g_wav_size += bytes_read;                         /* WAVData size increases */
                }
            }
            else 
            {
                vTaskDelay(1);
            }

            timecnt++;

            if ((timecnt % 20) == 0)
            {
                LED_TOGGLE();      /* LEDFlashing */
            }

            if (recsec != (g_wav_size / wavhead->fmt.ByteRate))    /* recordingtime显示 */
            {
                recsec = g_wav_size / wavhead->fmt.ByteRate;       /* recordingtime */
                recoder_msg_show(recsec, wavhead->fmt.SampleRate * wavhead->fmt.NumOfChannels * wavhead->fmt.BitsPerSample); /* Display code rate */
            }
        }
    }

    free(pdatabuf);       /* Free memory */
    free(f_rec);          /* Free memory */
    free(wavhead);        /* Free memory */
    free(pname);          /* Free memory */
}
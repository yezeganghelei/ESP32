/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file wavplay.c
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief wav decoding code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "wavplay.h"
/*FreeRTOS*********************************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/******************************************************************************************************/
/*FreeRTOSConfiguration*/

/* MUSIC Task Configuration
 * include: Task handle Taskpriority stack size createTask
 */
#define MUSIC_PRIO      4                   /* Taskpriority */
#define MUSIC_STK_SIZE  5*1024              /* Task stack size */
TaskHandle_t            MUSICTask_Handler;  /* Task handle */
void music(void *pvParameters);             /* Task functions */

static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;

/******************************************************************************************************/

__wavctrl wavctrl;                          /* WAVControl structure */

esp_err_t i2s_play_end = ESP_FAIL;
esp_err_t i2s_play_next_prev = ESP_FAIL;

/**
 * @brief       WAVAnalytical Initialization
 * @param       fname : File path+documentname
 * @param       wavx  : Information storage structure pointer
 * @retval      0,Open the filesuccess
 *              1,Open the filefail
 *              2,NoWAVdocument
 *              3,DATAArea not found
 */
uint8_t wav_decode_init(uint8_t *fname, __wavctrl *wavx)
{
    FIL *ftemp;
    uint8_t *buf; 
    uint32_t br = 0;
    uint8_t res = 0;

    ChunkRIFF *riff;
    ChunkFMT *fmt;
    ChunkFACT *fact;
    ChunkDATA *data;
    
    ftemp = (FIL*)malloc(sizeof(FIL));
    buf = malloc(512);
    
    if (ftemp && buf)                                           /* Memory application was successful */
    {
        res = f_open(ftemp, (TCHAR*)fname, FA_READ);            /* Open the file */
        
        if (res == FR_OK)
        {
            f_read(ftemp, buf, 512, (UINT *)&br);               /* Read512Bytes in data */
            riff = (ChunkRIFF *)buf;                            /* GetRIFFpiece */
            
            if (riff->Format == 0x45564157)                     /* It's a WAV file */
            {
                fmt = (ChunkFMT *)(buf + 12);                   /* GetFMTpiece */
                fact = (ChunkFACT *)(buf + 12 + 8 + fmt->ChunkSize);                    /* ReadFACTpiece */
                
                if (fact->ChunkID == 0x74636166 || fact->ChunkID == 0x5453494C)
                {
                    wavx->datastart = 12 + 8 + fmt->ChunkSize + 8 + fact->ChunkSize;    /* havefact/LISTWhen blocking(Not tested) */
                }
                else
                {
                    wavx->datastart = 12 + 8 + fmt->ChunkSize;
                }
                
                data = (ChunkDATA *)(buf + wavx->datastart);    /* Reading DATA blocks */
                
                if (data->ChunkID == 0x61746164)                /* The analysis was successful! */
                {
                    wavx->audioformat = fmt->AudioFormat;       /* Audio format */
                    wavx->nchannels = fmt->NumOfChannels;       /* Number of channels */
                    wavx->samplerate = fmt->SampleRate;         /* Sampling rate */
                    wavx->bitrate = fmt->ByteRate * 8;          /* Get the speed */
                    wavx->blockalign = fmt->BlockAlign;         /* Block Alignment */
                    wavx->bps = fmt->BitsPerSample;             /* digit number, 16/24/32 bits */
                    
                    wavx->datasize = data->ChunkSize;           /* Data block size */
                    wavx->datastart = wavx->datastart + 8;      /* Where the data flow begins. */
                     
                    printf("wavx->audioformat:%d\r\n", wavx->audioformat);
                    printf("wavx->nchannels:%d\r\n", wavx->nchannels);
                    printf("wavx->samplerate:%ld\r\n", wavx->samplerate);
                    printf("wavx->bitrate:%ld\r\n", wavx->bitrate);
                    printf("wavx->blockalign:%d\r\n", wavx->blockalign);
                    printf("wavx->bps:%d\r\n", wavx->bps);
                    printf("wavx->datasize:%ld\r\n", wavx->datasize);
                    printf("wavx->datastart:%ld\r\n", wavx->datastart);  
                }
                else
                {
                    res = 3;                                    /* data area not found. */
                }
            }
            else
            {
                res = 2;        /* Non-wav files */
            }
        }
        else
        {
            res = 1;            /* Open the filemistake */
        }
    }
    
    f_close(ftemp);             /* Close the file */
    free(ftemp);                /* Free memory */
    free(buf); 
    
    return 0;
}

/**
 * @brief Get the current playback time
 * @param fx : File pointer
 * @param wavx : wavx playback controller
 * @retval None
 */
void wav_get_curtime(FIL *fx, __wavctrl *wavx)
{
    long long fpos;

    wavx->totsec = wavx->datasize / (wavx->bitrate / 8);    /* Total song length(oneBit:Second) */
    fpos = fx->fptr-wavx->datastart;                        /* Get where the current file is played */
    wavx->cursec = fpos * wavx->totsec / wavx->datasize;    /* How many seconds has it been played? */
}

/**
 * @brief music task
 * @param pvParameters: Pass in parameters (not used)
 * @retval None
 */
void music(void *pvParameters)
{
    pvParameters = pvParameters;
    uint32_t nr;
    FSIZE_t res = 0;
    uint8_t n = 0;
    volatile long long int i2s_table_size = 0;

    while(1)
    {
        if ((g_audiodev.status & 0x0F) == 0x03)
        {
            f_lseek(g_audiodev.file, n ? res : wavctrl.datastart);                          /* Skip file header */

            for (uint16_t readTimes = 0; readTimes < (wavctrl.datasize / WAV_TX_BUFSIZE); readTimes++)
            {
                if ((g_audiodev.status & 0x0F) == 0x00)                                     /* Pause playback */
                {
                    res = f_tell(g_audiodev.file);                                          /* Record pause position */
                    n = 1;
                    break;
                }

                if (i2s_table_size >= wavctrl.datasize || i2s_play_next_prev == ESP_OK)   /* Whether the playback is complete */
                {
                    n = 0;
                    i2s_table_size = 0;
                    i2s_play_end = ESP_OK;
                    audio_stop();
                    vTaskDelay(10);
                    break;
                }

                f_read(g_audiodev.file,g_audiodev.tbuf, WAV_TX_BUFSIZE, (UINT*)&nr);        /* Read the file */
                i2s_table_size = i2s_table_size + i2s_tx_write(g_audiodev.tbuf, WAV_TX_BUFSIZE);
                vTaskDelay(1);
            }

        }
        else
        {
            vTaskDelay(10);
        }
    }
}

/**
 * @brief       Play a certainwavdocument
 * @param       fname : File path+documentname
 * @retval      KEY0_PRES,mistake
 *              KEY1_PRES,Open the filefail
 *              other,NoWAVdocument
 */
uint8_t wav_play_song(uint8_t *fname)
{
    uint8_t key = 0;
    uint8_t t = 0;
    uint8_t res;
    i2s_play_end = ESP_FAIL;
    i2s_play_next_prev = ESP_FAIL;
    g_audiodev.file = (FIL*)malloc(sizeof(FIL));
    g_audiodev.tbuf = malloc(WAV_TX_BUFSIZE);
    
    if (g_audiodev.file || g_audiodev.tbuf)
    {
        res = wav_decode_init(fname, &wavctrl);                                 /* Get document information */

        if (res == 0)                                                           /* Successfully parsed the file */
        {
            if (wavctrl.bps == 16)
            {
                es8388_sai_cfg(0, 3);                                           /* Philips Standard,16Number of digitsAccording to length */
                i2s_set_samplerate_bits_sample(wavctrl.samplerate,I2S_BITS_PER_SAMPLE_16BIT);
            }
            else if (wavctrl.bps == 24)
            {
                es8388_sai_cfg(0, 0);                                           /* Philips Standard, 24 Number of digits according to length */
                i2s_set_samplerate_bits_sample(wavctrl.samplerate,I2S_BITS_PER_SAMPLE_24BIT);
             }

            audio_stop();

            if (MUSICTask_Handler == NULL)
            {
                taskENTER_CRITICAL(&my_spinlock);
                /* Create Task 1 */
                xTaskCreatePinnedToCore((TaskFunction_t )music,                 /* Task functions */
                                        (const char*    )"music",               /* Task name */
                                        (uint16_t       )MUSIC_STK_SIZE,        /* Task stack size */
                                        (void*          )NULL,                  /* Passed in toTask functionsParameters of */
                                        (UBaseType_t    )MUSIC_PRIO,            /* Taskpriority */
                                        (TaskHandle_t*  )&MUSICTask_Handler,    /* Task handle */
                                        (BaseType_t     ) 0);                   /* Which kernel does this task run */
                taskEXIT_CRITICAL(&my_spinlock);
            }

            res = f_open(g_audiodev.file, (TCHAR*)fname, FA_READ);              /* Open the file */

            if (res == 0)
            {
                audio_start();                                                  /* Start audio playback */
                vTaskDelay(100);
                audio_start();
                vTaskDelay(100);

                while (res == 0)
                { 
                    while (1)
                    {
                        if (i2s_play_end == ESP_OK)
                        {
                            res = KEY0_PRES;
                            break;
                        }

                        key = xl9555_key_scan(0);
                        
                        if (key == KEY3_PRES)                                   /* pause */
                        {
                            if ((g_audiodev.status & 0x0F) == 0x03)
                            {
                                audio_stop();
                                vTaskDelay(100);
                            }
                            else if ((g_audiodev.status & 0x0F) == 0x00)
                            {
                                audio_start();
                                vTaskDelay(100);
                            }
                        }
                        
                        if (key == KEY2_PRES || key == KEY0_PRES)               /* Next song/Previous song */
                        {
                            i2s_play_next_prev = ESP_OK;
                            vTaskDelay(100);
                            res = KEY0_PRES;
                            break;
                        }
                        
                        if ((g_audiodev.status & 0x0F) == 0x03)                 /* Pause not refresh time */
                        {
                            wav_get_curtime(g_audiodev.file, &wavctrl);         /* Get the total time and the current playback time */
                            audio_msg_show(wavctrl.totsec, wavctrl.cursec, wavctrl.bitrate);
                        }
                        
                        t++;
                        if (t == 20)
                        {
                            t = 0 ;
                            LED_TOGGLE();
                        }
                        
                        if ((g_audiodev.status & 0x01) == 0)
                        {
                            vTaskDelay(10);
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (key == KEY2_PRES || key == KEY0_PRES)                   /* Exit to switch songs */
                    {
                        res = key;
                        break;
                    }
                }
                audio_stop();
            }
            else
            {
                res = 0xFF;
            }
        }
        else
        {
            res = 0xFF;
        }
    }
    else
    {
        res = 0xFF;
    }
    
    free(g_audiodev.tbuf);                                                      /* Free memory */
    free(g_audiodev.file);                                                      /* Free memory */
    
    return res;
}
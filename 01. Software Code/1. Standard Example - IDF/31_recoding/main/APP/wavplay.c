/**
 ****************************************************************************************************
 * @file        wavplay.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       wavdecoding Code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "wavplay.h"
/*FreeRTOS*********************************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/******************************************************************************************************/
/*FreeRTOSConfiguration*/

/* MUSIC Task Configuration
 * include: task handle, task priority, stack size, create task
 */
#define MUSIC_PRIO      4                   /* Task priority */
#define MUSIC_STK_SIZE  5*1024              /* Task stack size */
TaskHandle_t            MUSICTask_Handler;  /* Task handle */
void music(void *pvParameters);             /* Task function */

static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;

/******************************************************************************************************/

__wavctrl wavctrl;                          /* WAVControl structure */

esp_err_t i2s_play_end = ESP_FAIL;
esp_err_t i2s_play_next_prev = ESP_FAIL;

/**
 * @brief       WAVAnalytical Initialization
 * @param       fname : file path + file name
 * @param       wavx  : Information storage structure pointer
 * @retval      0, file opened successfully
 *              1, failed to open file
 *              2, no WAV file
 *              3, DATA area not found
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
        res = f_open(ftemp, (TCHAR*)fname, FA_READ);            /* Open file */
        
        if (res == FR_OK)
        {
            f_read(ftemp, buf, 512, (UINT *)&br);               /* Read 512 bytes in the data */
            riff = (ChunkRIFF *)buf;                            /* GetRIFFpiece */
            
            if (riff->Format == 0x45564157)                     /* yesWAVdocument */
            {
                fmt = (ChunkFMT *)(buf + 12);                   /* GetFMTpiece */
                fact = (ChunkFACT *)(buf + 12 + 8 + fmt->ChunkSize);                    /* ReadFACTpiece */
                
                if (fact->ChunkID == 0x74636166 || fact->ChunkID == 0x5453494C)
                {
                    wavx->datastart = 12 + 8 + fmt->ChunkSize + 8 + fact->ChunkSize;    /* when a fact/LIST chunk is present (not tested) */
                }
                else
                {
                    wavx->datastart = 12 + 8 + fmt->ChunkSize;
                }
                
                data = (ChunkDATA *)(buf + wavx->datastart);    /* ReadDATApiece */
                
                if (data->ChunkID == 0x61746164)                /* The analysis was successful! */
                {
                    wavx->audioformat = fmt->AudioFormat;       /* Audio format */
                    wavx->nchannels = fmt->NumOfChannels;       /* Number of channels */
                    wavx->samplerate = fmt->SampleRate;         /* Sampling rate */
                    wavx->bitrate = fmt->ByteRate * 8;          /* Get bit rate */
                    wavx->blockalign = fmt->BlockAlign;         /* chunk alignment */
                    wavx->bps = fmt->BitsPerSample;             /* digit number, 16/24/32 bits */
                    
                    wavx->datasize = data->ChunkSize;           /* data chunk size */
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
                    res = 3;                                    /* dataArea not found. */
                }
            }
            else
            {
                res = 2;        /* Nowavdocument */
            }
        }
        else
        {
            res = 1;            /* Failed to open file */
        }
    }
    
    f_close(ftemp);             /* Close file */
    free(ftemp);                /* Free memory */
    free(buf); 
    
    return 0;
}

/**
 * @brief       Get the current playback time
 * @param       fx    : File pointer
 * @param       wavx  : wavxPlayback Controller
 * @retval      none
 */
void wav_get_curtime(FIL *fx, __wavctrl *wavx)
{
    long long fpos;

    wavx->totsec = wavx->datasize / (wavx->bitrate / 8);    /* Total song length(unit:Second) */
    fpos = fx->fptr-wavx->datastart;                        /* Get where the current file is played */
    wavx->cursec = fpos * wavx->totsec / wavx->datasize;    /* How many seconds has it been played? */
}

/**
 * @brief       musicTask
 * @param       pvParameters : Pass in parameters(Not used)
 * @retval      none
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
            f_lseek(g_audiodev.file, n ? res : wavctrl.datastart);                          /* Skip the file header */

            for (uint16_t readTimes = 0; readTimes < (wavctrl.datasize / WAV_TX_BUFSIZE); readTimes++)
            {
                if ((g_audiodev.status & 0x0F) == 0x00)                                     /* Pause playback */
                {
                    res = f_tell(g_audiodev.file);                                          /* Record the pause position */
                    n = 1;
                    break;
                }

                if (i2s_table_size >= wavctrl.datasize || i2s_play_next_prev == ESP_OK)   /* Whether playback is complete */
                {
                    n = 0;
                    i2s_table_size = 0;
                    i2s_play_end = ESP_OK;
                    audio_stop();
                    vTaskDelay(10);
                    break;
                }

                f_read(g_audiodev.file,g_audiodev.tbuf, WAV_TX_BUFSIZE, (UINT*)&nr);        /* Read file */
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
 * @param       fname : file path + file name
 * @retval      KEY0_PRES, mistake
 *              KEY1_PRES, failed to open file
 *              other, no WAV file
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
        res = wav_decode_init(fname, &wavctrl);     /* Get file information */

        if (res == 0)                               /* File parsed successfully */
        {
            if (wavctrl.bps == 16)
            {
                es8388_sai_cfg(0, 3);               /* Philips standard, 16-bit data length */
                i2s_set_samplerate_bits_sample(wavctrl.samplerate,I2S_BITS_PER_SAMPLE_16BIT);
            }
            else if (wavctrl.bps == 24)
            {
                es8388_sai_cfg(0, 0);               /* Philips standard, 24-bit data length */
                i2s_set_samplerate_bits_sample(wavctrl.samplerate,I2S_BITS_PER_SAMPLE_24BIT);
             }

            audio_stop();

            if (MUSICTask_Handler == NULL)
            {
                taskENTER_CRITICAL(&my_spinlock);
                /* Create task 1 */
                xTaskCreatePinnedToCore((TaskFunction_t )music,                 /* Task function */
                                        (const char*    )"music",               /* Task name */
                                        (uint16_t       )MUSIC_STK_SIZE,        /* Task stack size */
                                        (void*          )NULL,                  /* Parameter passed to the task function */
                                        (UBaseType_t    )MUSIC_PRIO,            /* Task priority */
                                        (TaskHandle_t*  )&MUSICTask_Handler,    /* Task handle */
                                        (BaseType_t     ) 0);                   /* Which core the task runs on */
                taskEXIT_CRITICAL(&my_spinlock);
            }

            res = f_open(g_audiodev.file, (TCHAR*)fname, FA_READ);  /* Open file */

            if (res == 0)
            {
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
                        
                        if (key == KEY3_PRES)   /* pause */
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
                        
                        if (key == KEY2_PRES || key == KEY0_PRES)   /* Next song/Previous song */
                        {
                            i2s_play_next_prev = ESP_OK;
                            vTaskDelay(100);
                            res = KEY0_PRES;
                            break;
                        }
                        
                        if ((g_audiodev.status & 0x0F) == 0x03)     /* Pause not refresh time */
                        {
                            wav_get_curtime(g_audiodev.file, &wavctrl); /* Get the total time and the current playing time */
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

                    if (key == KEY2_PRES || key == KEY0_PRES)   /* Exit to switch songs */
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
    
    free(g_audiodev.tbuf);      /* Free memory */
    free(g_audiodev.file);      /* Free memory */
    
    return res;
}
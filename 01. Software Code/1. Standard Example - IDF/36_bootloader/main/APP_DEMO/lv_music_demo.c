/**
 ****************************************************************************************************
 * @file        lv_music.c
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-11-04
 * @brief       Music player
 * @license     Copyright (c) 2020-2032, Guangzhou Xingyi Electronic Technology Co., Ltd.
 ****************************************************************************************************
 * @attention
 *
 * Platform: ALIENTEK ESP32-S3 development board
 * Online video: www.yuanzige.com
 * Technical forum: www.openedv.com
 * Company website: www.alientek.com
 * Purchase: openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "lv_music_demo.h"


LV_FONT_DECLARE(myFont12)           /* Declare the myFont24 font, which is already flashed to SPIFLASH */
LV_IMG_DECLARE(ui_img_album_png);

lv_obj_t *lv_music_cont;            /* Main window */
lv_obj_t *lv_music_client_cont;     /* Window child container */
lv_obj_t *lv_music_win_header;      /* Window title */

static lv_obj_t *song_play_label;
static lv_obj_t *song_last_label;
static lv_obj_t *song_next_label;

static lv_obj_t *obj_cont_4;
static lv_obj_t *obj_cont_5;
static lv_obj_t *obj_cont_6;

static lv_obj_t *now_time_label;
static lv_obj_t *total_time_label;
static lv_obj_t *time_slider;
static lv_obj_t * song_name;

__wavctrl wavctrl;                          /* WAV control structure */
__audiodev g_audiodev;                      /* Music playback controller */
uint8_t song_play_state = 0;
uint16_t curindex;                          /* Current index */
FILINFO *wavfileinfo;                       /* File information */
uint8_t *pname;                             /* File name with path */
uint32_t *wavoffsettbl;                     /* Music offset index table */
FF_DIR wavdir;                              /* Directory */
uint16_t totwavnum;                         /* Total number of music files */
uint8_t music_key = 0;
esp_err_t i2s_play_end = ESP_FAIL;
esp_err_t i2s_play_next_prev = ESP_FAIL;
uint32_t nr = 0;
FSIZE_t lv_res = 0;
uint8_t n = 0;
volatile long long int i2s_table_size = 0;
extern uint8_t sd_check_en;                 /* SD card detection flag */
/* MUSIC task configuration
 * Includes: task handle, task priority, stack size, task creation
 */
#define MUSIC_PRIO      2                   /* Task priority */
#define MUSIC_STK_SIZE  5*1024              /* Task stack size */
TaskHandle_t            MUSICTask_Handler;  /* Task handle */
void music(void *pvParameters);             /* Task function */


/* PLAY task configuration
 * Includes: task handle, task priority, stack size, task creation
 */
#define PLAY_PRIO      10                  /* Task priority */
#define PLAY_STK_SIZE  5*1024              /* Task stack size */
TaskHandle_t           PLAYTask_Handler;   /* Task handle */
void plsy(void *pvParameters);             /* Task function */


/**
 * @brief       Start audio playback
 * @param       none
 * @retval      none
 */
void lv_audio_start(void)
{
    g_audiodev.status = 3 << 0; /* Start playback + not paused */
    i2s_trx_start();
}

/**
 * @brief       Stop audio playback
 * @param       none
 * @retval      none
 */
void lv_audio_stop(void)
{
    g_audiodev.status = 0;
    i2s_trx_stop();
}

/**
 * @brief       Display playback time and bitrate information
 * @param       totsec : total audio file duration
 * @param       cursec : current playback time
 * @param       bitrate: bitrate
 * @retval      none
 */
void audio_msg_show(uint32_t totsec, uint32_t cursec, uint32_t bitrate)
{
    static uint16_t playtime = 0xFFFF;                                  /* Playback time flag */
    
    if (playtime != cursec)                                             /* Display time needs updating */
    {
        playtime = cursec;
        lv_slider_set_range(time_slider,0,totsec);
        lv_slider_set_value(time_slider,playtime,LV_ANIM_ON);
        lv_label_set_text_fmt(now_time_label, "%d:%d", playtime / 60,playtime % 60);  /* Get the current value and update the display */
        lv_label_set_text_fmt(total_time_label, "%ld:%ld", totsec / 60,totsec % 60);  /* Get the current value and update the display */
    }
}

/**
 * @brief  Music playback event callback
 * @param  *e : collection of event-related parameters containing all event data
 * @return none
 */
static void song_play_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                                         /* Get the trigger source */
    lv_event_code_t code = lv_event_get_code(e);                                       /* Get the event type */
    
    if (target == obj_cont_4)        /* Start, stop */
    {
        if (code == LV_EVENT_CLICKED)
        {
            if (song_play_state ==0)
            {
                music_key = MUSIC_PLAY;
                lv_audio_stop();
                lv_label_set_text(song_play_label, LV_SYMBOL_PLAY);
                song_play_state = 1;
            }
            else
            {
                music_key = MUSIC_PAUSE;
                lv_audio_start();
                lv_label_set_text(song_play_label, LV_SYMBOL_PAUSE);
                song_play_state = 0;
            }
        }
    }
    else if (target == obj_cont_5)       /* Next song */
    {
        if (code == LV_EVENT_PRESSED)
        {
            lv_obj_set_style_text_color(song_last_label, lv_color_hex(0x272727), 0);
        }
        else if (code == LV_EVENT_RELEASED)
        {
            lv_audio_stop();
            music_key = MUSIC_NEXT;
            lv_label_set_text(song_play_label, LV_SYMBOL_PAUSE);
            lv_obj_set_style_text_color(song_last_label, lv_color_hex(0xffffff), 0);
            nr = 0;
            lv_res = 0;
            n = 0;
            i2s_table_size = 0;
            song_play_state = 0;
        }
    }
    else if (target == obj_cont_6)       /* Previous song */
    {
        if (code == LV_EVENT_PRESSED)
        {
            lv_obj_set_style_text_color(song_next_label, lv_color_hex(0x272727), 0);
        }
        else if (code == LV_EVENT_RELEASED)
        {
            lv_audio_stop();
            music_key = MUSIC_PREV;
            lv_label_set_text(song_play_label, LV_SYMBOL_PAUSE);
            lv_obj_set_style_text_color(song_next_label, lv_color_hex(0xffffff), 0);
            nr = 0;
            lv_res = 0;
            n = 0;
            i2s_table_size = 0;
            song_play_state = 0;
        }
    }
}

/**
 * @brief       WAV parsing initialization
 * @param       fname : file path + file name
 * @param       wavx  : pointer to the structure that stores the information
 * @retval      0, file opened successfully
 *              1, failed to open file
 *              2, not a WAV file
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
    
    if (ftemp && buf)                                           /* Memory allocated successfully */
    {
        res = f_open(ftemp, (TCHAR*)fname, FA_READ);            /* Open the file */
        
        if (res == FR_OK)
        {
            f_read(ftemp, buf, 512, (UINT *)&br);               /* Read 512 bytes of data */
            riff = (ChunkRIFF *)buf;                            /* Get the RIFF chunk */
            
            if (riff->Format == 0x45564157)                     /* It is a WAV file */
            {
                fmt = (ChunkFMT *)(buf + 12);                   /* Get the FMT chunk */
                fact = (ChunkFACT *)(buf + 12 + 8 + fmt->ChunkSize);                    /* Read the FACT chunk */
                
                if (fact->ChunkID == 0x74636166 || fact->ChunkID == 0x5453494C)
                {
                    wavx->datastart = 12 + 8 + fmt->ChunkSize + 8 + fact->ChunkSize;    /* When a fact/LIST chunk is present (untested) */
                }
                else
                {
                    wavx->datastart = 12 + 8 + fmt->ChunkSize;
                }
                
                data = (ChunkDATA *)(buf + wavx->datastart);    /* Read the DATA chunk */
                
                if (data->ChunkID == 0x61746164)                /* Parsing succeeded! */
                {
                    wavx->audioformat = fmt->AudioFormat;       /* Audio format */
                    wavx->nchannels = fmt->NumOfChannels;       /* Number of channels */
                    wavx->samplerate = fmt->SampleRate;         /* Sample rate */
                    wavx->bitrate = fmt->ByteRate * 8;          /* Get the bitrate */
                    wavx->blockalign = fmt->BlockAlign;         /* Block alignment */
                    wavx->bps = fmt->BitsPerSample;             /* Bit depth, 16/24/32-bit */
                    
                    wavx->datasize = data->ChunkSize;           /* Data chunk size */
                    wavx->datastart = wavx->datastart + 8;      /* Where the data stream starts. */
                     
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
                    res = 3;                                    /* DATA area not found */
                }
            }
            else
            {
                res = 2;        /* Not a WAV file */
            }
        }
        else
        {
            res = 1;            /* File open error */
        }
    }
    
    f_close(ftemp);             /* Close the file */
    free(ftemp);                /* Free memory */
    free(buf); 
    
    return 0;
}


/**
  * @brief  Delete the music demo
  * @param  none
  * @retval none
  */
void lv_music_del(void)
{
    lv_audio_stop();
    f_close(g_audiodev.file);       /* Close the file */

    if (PLAYTask_Handler != NULL)
    {
        vTaskDelete(PLAYTask_Handler);
        taskYIELD();
    }

    if (wavfileinfo || pname || wavoffsettbl)
    {
        free(wavfileinfo);                                      /* Free memory */
        free(pname);                                            /* Free memory */
        free(wavoffsettbl);                                     /* Free memory */
    }

    if (g_audiodev.file || g_audiodev.tbuf)
    {
        free(g_audiodev.file);
        free(g_audiodev.tbuf);
    }

    if (MUSICTask_Handler != NULL)
    {
        vTaskDelete(MUSICTask_Handler);
        taskYIELD();
    }

    PLAYTask_Handler = NULL;
    MUSICTask_Handler = NULL;

    es8388_adda_cfg(0,0);                               /* Enable DAC, disable ADC */
    es8388_input_cfg(0);                                /* Disable input */
    es8388_output_cfg(0,0);                             /* Select DAC channel output */
    i2s_deinit();
    xl9555_pin_write(SPK_EN_IO,1);                      /* Turn off the speaker */
    lv_obj_clean(app_obj_general.current_parent);
    lv_obj_del(app_obj_general.current_parent);
    app_obj_general.current_parent = NULL;
    lv_app_show();
    xSemaphoreGive(xGuiSemaphore);                      /* Release the mutex semaphore */
}

/**
 * @brief       play
 * @param       pvParameters : passed-in parameter (unused)
 * @retval      none
 */
void play(void *pvParameters)
{
    pvParameters = pvParameters;

    while(1)
    {
        if ((g_audiodev.status & 0x0F) == 0x03)
        {
            f_lseek(g_audiodev.file, n ? lv_res : wavctrl.datastart);                          /* Skip the file header */

            for (uint16_t readTimes = 0; readTimes < (wavctrl.datasize / WAV_TX_BUFSIZE); readTimes++)
            {
                if ((g_audiodev.status & 0x0F) == 0x00)                                     /* Pause playback */
                {
                    if (music_key == MUSIC_PLAY || music_key == MUSIC_PAUSE)
                    {
                        lv_res = f_tell(g_audiodev.file);                                          /* Record the pause position */
                        n = 1;
                    }

                    i2s_zero_dma_buffer(I2S_NUM);

                    if (music_key == 0xFF)
                    {
                        nr = 0;
                        lv_res = 0;
                        n = 0;
                        i2s_table_size = 0;
                        song_play_state = 0;
                        break;
                    }

                    break;
                }

                if (i2s_table_size >= wavctrl.datasize || i2s_play_next_prev == ESP_OK)   /* Check whether playback is complete */
                {
                    n = 0;
                    i2s_table_size = 0;
                    i2s_play_end = ESP_OK;
                    i2s_zero_dma_buffer(I2S_NUM);
                    taskYIELD();
                    break;
                }
                /* Handle SD card removal while paused */
                if (sdmmc_get_status(card) != ESP_OK)
                {
                    i2s_zero_dma_buffer(I2S_NUM);
                    g_audiodev.status = 0;

                    while (1)
                    {
                        back_act_key = KEY1_PRES;
                        taskYIELD();
                    }
                }

                f_read(g_audiodev.file,g_audiodev.tbuf, WAV_TX_BUFSIZE, (UINT*)&nr);        /* Read the file */
                i2s_table_size = i2s_table_size + i2s_tx_write(g_audiodev.tbuf, WAV_TX_BUFSIZE);
                vTaskDelay(10);
            }
        }
        else
        {
            /* Handle SD card removal while paused */
            if (sdmmc_get_status(card) != ESP_OK)
            {
                while (1)
                {
                    back_act_key = KEY1_PRES;
                    vTaskDelay(10);
                }
            }

            vTaskDelay(10);
        }
    }
}

/**
 * @brief       Get the current playback time
 * @param       fx    : file pointer
 * @param       wavx  : wavx playback controller
 * @retval      none
 */
void wav_get_curtime(FIL *fx, __wavctrl *wavx)
{
    long long fpos;

    wavx->totsec = wavx->datasize / (wavx->bitrate / 8);    /* Total song length (unit: seconds) */
    fpos = fx->fptr-wavx->datastart;                        /* Get the current playback position in the file */
    wavx->cursec = fpos * wavx->totsec / wavx->datasize;    /* Which second is currently playing? */
}

/**
 * @brief       Get the total number of target files under the path
 * @param       path : file path
 * @retval      Total number of valid files
 */
uint16_t audio_get_tnum(uint8_t *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;                                                /* Temporary directory */
    FILINFO *tfileinfo;                                         /* Temporary file information */
    
    tfileinfo = (FILINFO*)malloc(sizeof(FILINFO));              /* Allocate memory */
    
    res = f_opendir(&tdir, (const TCHAR*)path);                 /* Open the directory */
    
    if ((res == FR_OK) && tfileinfo)
    {
        while (1)                                               /* Query the total number of valid files */
        {
            res = f_readdir(&tdir, tfileinfo);                  /* Read one file in the directory */
            
            if ((res != FR_OK) || (tfileinfo->fname[0] == 0))
            {
                break;                                          /* On error or end, exit */
            }

            res = exfuns_file_type(tfileinfo->fname);
            
            if ((res & 0xF0) == 0x40)                           /* Take the high nibble to check whether it is a music file */
            {
                rval++;                                         /* Increment the valid file count */
            }
        }
    }
    
    free(tfileinfo);                                            /* Free memory */
    
    return rval;
}

/**
 * @brief       music
 * @param       pvParameters : passed-in parameter (unused)
 * @retval      none
 */
void music(void *pvParameters)
{
    pvParameters = pvParameters;
    uint8_t res;
    uint32_t temp;
    g_audiodev.file = (FIL*)malloc(sizeof(FIL));
    g_audiodev.tbuf = malloc(WAV_TX_BUFSIZE);

    es8388_adda_cfg(1, 0);                              /* Enable DAC, disable ADC */
    es8388_input_cfg(0);                                /* Disable input */
    es8388_output_cfg(1, 1);                            /* Select DAC channel output */
    es8388_hpvol_set(20);                               /* Set headphone volume */
    es8388_spkvol_set(20);                              /* Set speaker volume */
    xl9555_pin_write(SPK_EN_IO,0);                      /* Turn on the speaker */

    while(1)
    {
        /* Record the index */
        res = f_opendir(&wavdir, "0:/MUSIC");                       /* Open the directory */

        if (res == FR_OK)
        {
            curindex = 0;                                           /* Current index is 0 */
            
            while (1)                                               /* Query everything once */
            {
                temp = wavdir.dptr;                                 /* Record the current index */

                res = f_readdir(&wavdir, wavfileinfo);              /* Read one file in the directory */
                
                if ((res != FR_OK) || (wavfileinfo->fname[0] == 0))
                {
                    break;                                          /* On error or end, exit */
                }

                res = exfuns_file_type(wavfileinfo->fname);
                
                if ((res & 0xF0) == 0x40)                           /* Take the high nibble to check whether it is a music file */
                {
                    wavoffsettbl[curindex] = temp;                   /* Record the index */
                    curindex++;
                }
            }
        }

        curindex = 0;                                               /* Display starting from 0 */
        res = f_opendir(&wavdir, (const TCHAR*)"0:/MUSIC");         /* Open the directory */
        
        while (res == FR_OK)                                        /* Opened successfully */
        {
            dir_sdi(&wavdir, wavoffsettbl[curindex]);               /* Change the current directory index */
            res = f_readdir(&wavdir, wavfileinfo);                  /* Read one file in the directory */
            
            if ((res != FR_OK) || (wavfileinfo->fname[0] == 0))
            {
                break;                                              /* On error or end, exit */
            }
            
            strcpy((char *)pname, "0:/MUSIC/");                     /* Copy the path (directory) */
            strcat((char *)pname, (const char *)wavfileinfo->fname);/* Append the file name */
            lv_label_set_text_fmt(song_name,"%s",wavfileinfo->fname);
            i2s_play_end = ESP_FAIL;
            i2s_play_next_prev = ESP_FAIL;
            music_key = 0;
            
            if (g_audiodev.file || g_audiodev.tbuf)
            {
                res = wav_decode_init(pname, &wavctrl);     /* Get the file information */
            }

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
            }

            res = f_open(g_audiodev.file, (TCHAR*)pname, FA_READ);  /* Open the file */

            lv_audio_stop();
            vTaskDelay(100);
            lv_audio_start();

            if (PLAYTask_Handler == NULL)
            {
                /* Create the MUSIC task */
                xTaskCreatePinnedToCore((TaskFunction_t )play,                 /* Task function */
                                        (const char*    )"play",               /* Task name */
                                        (uint16_t       )PLAY_STK_SIZE,        /* Task stack size */
                                        (void*          )NULL,                 /* Parameter passed to the task function */
                                        (UBaseType_t    )PLAY_PRIO,            /* Task priority */
                                        (TaskHandle_t*  )&PLAYTask_Handler,    /* Task handle */
                                        (BaseType_t     ) 1);                  /* Core on which the task runs */
            }

            while (1)
            {
                xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

                if (music_key == MUSIC_PREV)                                   /* Previous song */
                {
                    if (curindex)
                    {
                        curindex--;
                    }
                    else
                    {
                        curindex = totwavnum - 1;
                    }

                    i2s_play_next_prev = ESP_OK;
                    break;
                }
                else if (music_key == MUSIC_NEXT || i2s_play_end == ESP_OK) /* Next song */
                {
                    curindex++;

                    if (curindex >= totwavnum)
                    {
                        curindex = 0;                                   /* At the end, automatically restart from the beginning */
                    }

                    i2s_play_next_prev = ESP_OK;
                    break;
                }

                if ((g_audiodev.status & 0x0F) == 0x03)     /* Do not refresh the time while paused */
                {
                    wav_get_curtime(g_audiodev.file, &wavctrl); /* Get the total time and current playback time */
                    audio_msg_show(wavctrl.totsec, wavctrl.cursec, wavctrl.bitrate);
                }

                xSemaphoreGive(xGuiSemaphore); /* Release the mutex semaphore */

                vTaskDelay(100);
            }

            i2s_zero_dma_buffer(I2S_NUM);
            g_audiodev.status = 0;
            nr = 0;
            lv_res = 0;
            n = 0;
            i2s_table_size = 0;
            song_play_state = 0;
            music_key = 0xff;
            xSemaphoreGive(xGuiSemaphore); /* Release the mutex semaphore */
        }

        free(wavfileinfo);                                          /* Free memory */
        free(pname);                                                /* Free memory */
        free(wavoffsettbl);                                         /* Free memory */
    }
}

/**
 * @brief  Music demo
 * @param  none
 * @return none
 */
void lv_music_demo(void)
{
    nr = 0;
    lv_res = 0;
    n = 0;
    i2s_table_size = 0;
    song_play_state = 0;

    /* Handle repeated presses */
    if (app_obj_general.current_parent != NULL)
    {
        lv_obj_del(app_obj_general.current_parent);
        app_obj_general.current_parent = NULL;
    }

    if (sd_check_en == 0)
    {
        lv_msgbox("SD device not detected");
    }
    else
    {
        sd_check_en = 1;        /* SD card inserted */
        i2s_init();
        vTaskDelay(100);
        if (f_opendir(&wavdir, "0:/MUSIC"))                      /* Open the music folder */
        {
            lv_msgbox("MUSIC folder error");
            return ;
        }
        
        totwavnum = audio_get_tnum((uint8_t *)"0:/MUSIC");/* Get the total number of valid files */

        if (totwavnum == 0)
        {
            lv_msgbox("No music files");
            return ;
        }

        wavfileinfo = (FILINFO*)malloc(sizeof(FILINFO));            /* Allocate memory */
        pname = malloc(255 * 2 + 1);                                /* Allocate memory for the file name with path */
        wavoffsettbl = malloc(4 * totwavnum);                       /* Allocate 4*totwavnum bytes to store the music file offset block index */

        if (!wavfileinfo || !pname || !wavoffsettbl)                /* Memory allocation error */
        {
            lv_msgbox("memory allocation failed");
            return ;
        }

        lv_app_del();
        /* Create the main container of this screen */
        lv_obj_t *music_obj = lv_obj_create(lv_scr_act());
        lv_obj_set_style_radius(music_obj, 0, LV_STATE_DEFAULT);
        lv_obj_set_size(music_obj,lv_obj_get_width(lv_scr_act()),lv_obj_get_height(lv_scr_act()));
        lv_obj_set_style_border_opa(music_obj,LV_OPA_0,LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(music_obj,lv_color_make(0,0,0),LV_STATE_DEFAULT);
        lv_obj_set_pos(music_obj,0,0);
        lv_obj_clear_flag(music_obj, LV_OBJ_FLAG_SCROLLABLE);

        app_obj_general.current_parent = music_obj;                 /* Point to the current screen container */
        app_obj_general.Function = lv_music_del;                    /* Delete this screen */

        /****************************** Child container 1 ******************************/
        
        lv_obj_t *obj_cont_1 = lv_obj_create(music_obj);                                                /* Create child container */
        lv_obj_set_size(obj_cont_1, lv_obj_get_width(lv_scr_act()) - 90, lv_obj_get_width(lv_scr_act()) - 90);  /* Set size */
        lv_obj_set_style_bg_color(obj_cont_1, lv_color_make(50,52,67), LV_STATE_DEFAULT);               /* Set background color */
        lv_obj_align(obj_cont_1, LV_ALIGN_TOP_MID, 0, 50);                                              /* Set position */
        lv_obj_set_style_border_opa(obj_cont_1, 0, 0);                                                  /* Remove the border */
        lv_obj_set_style_radius(obj_cont_1, 200, 0);                                                    /* Set the corner radius */

        lv_obj_t * img_obj_cont_1 = lv_img_create(obj_cont_1);
        lv_obj_set_size(img_obj_cont_1,ui_img_album_png.header.w, ui_img_album_png.header.h);           /* Set size */
        lv_img_set_src(img_obj_cont_1,&ui_img_album_png);
        lv_obj_center(img_obj_cont_1);
        
        song_name = lv_label_create(music_obj);
        lv_obj_align(song_name,LV_ALIGN_TOP_MID,0,25);
        lv_obj_set_style_text_font(song_name,&myFont12,LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(song_name,lv_color_hex(0xffffff),LV_STATE_DEFAULT);
        lv_label_set_text(song_name," ");
        /****************************** Time bar ******************************/
        
        /* Time bar */
        time_slider = lv_slider_create(music_obj);
        lv_obj_set_size(time_slider, lv_obj_get_width(lv_scr_act())/2 + 50, 5);
        lv_obj_align_to(time_slider, obj_cont_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
        lv_slider_set_range(time_slider,0,100);
        lv_obj_set_style_bg_color(time_slider, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(time_slider, 255, 0);
        lv_obj_set_style_bg_color(time_slider, lv_color_make(192,192,192), 0);
        lv_obj_remove_style(time_slider, NULL, LV_PART_KNOB);

        /* Elapsed time label */
        now_time_label = lv_label_create(music_obj);
        lv_obj_set_style_text_font(now_time_label, &lv_font_montserrat_14, 0);                          /* Set font */
        lv_label_set_text(now_time_label, "0:00");
        lv_obj_set_style_text_color(now_time_label, lv_color_hex(0xffffff), 0);
        lv_obj_align_to(now_time_label, time_slider, LV_ALIGN_OUT_LEFT_MID, 0, 0);
        
        /* Total time label */
        total_time_label = lv_label_create(music_obj);
        lv_obj_set_style_text_font(total_time_label, &lv_font_montserrat_14, 0);                          /* Set font */
        lv_label_set_text(total_time_label, "0:00");
        lv_obj_set_style_text_color(total_time_label, lv_color_hex(0xffffff), 0);
        lv_obj_align_to(total_time_label, time_slider, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
        
        /****************************** Child container 4 ******************************/
        
        obj_cont_4 = lv_obj_create(music_obj);                                                  /* Create child container */
        lv_obj_set_size(obj_cont_4, 70, 70);                                                    /* Set size */
        lv_obj_set_style_bg_color(obj_cont_4, lv_color_make(20,20,20), LV_STATE_DEFAULT);       /* Set background color */
        lv_obj_align(obj_cont_4, LV_ALIGN_BOTTOM_MID, 0, -10);                                  /* Set position */
        lv_obj_set_style_border_color(obj_cont_4,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(obj_cont_4,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_translate_y(obj_cont_4, 5, LV_STATE_PRESSED);
        lv_obj_set_style_radius(obj_cont_4, 70, 0);                                             /* Set the corner radius */
        lv_obj_set_style_border_width(obj_cont_4,2,LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(obj_cont_4,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_shadow_width(obj_cont_4,10,LV_STATE_FOCUS_KEY);
        lv_obj_add_event_cb(obj_cont_4, song_play_event_cb, LV_EVENT_CLICKED, NULL);
        
        /* Play/pause label */
        song_play_label = lv_label_create(obj_cont_4);
        lv_obj_set_style_text_font(song_play_label, &lv_font_montserrat_14, 0);                 /* Set font */
        lv_label_set_text(song_play_label, LV_SYMBOL_PAUSE);
        lv_obj_set_style_text_color(song_play_label, lv_color_hex(0xffffff), 0);
        lv_obj_align(song_play_label, LV_ALIGN_CENTER, 0, 0);

        /****************************** Child container 5 ******************************/
        
        obj_cont_5 = lv_obj_create(music_obj);                                                  /* Create child container */
        lv_obj_set_size(obj_cont_5, 50, 50);                                                    /* Set size */
        lv_obj_align_to(obj_cont_5, obj_cont_4, LV_ALIGN_OUT_LEFT_MID, -20, 0);
        lv_obj_set_style_bg_color(obj_cont_5,lv_color_make(20,20,20),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(obj_cont_5,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(obj_cont_5,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_radius(obj_cont_5, 50, 0);                                             /* Set the corner radius */
        lv_obj_set_style_border_width(obj_cont_5,2,LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(obj_cont_5,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_shadow_width(obj_cont_5,10,LV_STATE_FOCUS_KEY);
        lv_obj_add_event_cb(obj_cont_5, song_play_event_cb, LV_EVENT_ALL, NULL);
        
        /* Previous song label */
        song_last_label = lv_label_create(obj_cont_5);
        lv_obj_set_style_text_font(song_last_label, &lv_font_montserrat_14, 0);                 /* Set font */
        lv_label_set_text(song_last_label, LV_SYMBOL_PREV);
        lv_obj_set_style_text_color(song_last_label, lv_color_hex(0xffffff), 0);
        lv_obj_align(song_last_label, LV_ALIGN_CENTER, 0, 0);

        /****************************** Child container 6 ******************************/
        
        obj_cont_6 = lv_obj_create(music_obj);                                                  /* Create child container */
        lv_obj_set_size(obj_cont_6, 50, 50);                                                    /* Set size */
        lv_obj_align_to(obj_cont_6, obj_cont_4, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
        lv_obj_set_style_bg_color(obj_cont_6,lv_color_make(20,20,20),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(obj_cont_6,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(obj_cont_6,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_radius(obj_cont_6, 50, 0);                                             /* Set the corner radius */
        lv_obj_set_style_border_width(obj_cont_6,2,LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(obj_cont_6,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_shadow_width(obj_cont_6,10,LV_STATE_FOCUS_KEY);
        lv_obj_add_event_cb(obj_cont_6, song_play_event_cb, LV_EVENT_ALL, NULL);
        
        /* Next song label */
        song_next_label = lv_label_create(obj_cont_6);
        lv_obj_set_style_text_font(song_next_label, &lv_font_montserrat_14, 0);                 /* Set font */
        lv_label_set_text(song_next_label, LV_SYMBOL_NEXT);
        lv_obj_set_style_text_color(song_next_label, lv_color_hex(0xffffff), 0);
        lv_obj_align(song_next_label, LV_ALIGN_CENTER, 0, 0);
    
        lv_group_add_obj(ctrl_g, obj_cont_5);
        lv_group_add_obj(ctrl_g, obj_cont_4);
        lv_group_add_obj(ctrl_g, obj_cont_6);
        lv_group_remove_obj(time_slider);
        lv_group_focus_obj(obj_cont_5);                                     /* Focus */

        if (MUSICTask_Handler == NULL)
        {
            /* Create the MUSIC task */
            xTaskCreatePinnedToCore((TaskFunction_t )music,                 /* Task function */
                                    (const char*    )"music",               /* Task name */
                                    (uint16_t       )MUSIC_STK_SIZE,        /* Task stack size */
                                    (void*          )NULL,                  /* Parameter passed to the task function */
                                    (UBaseType_t    )MUSIC_PRIO,            /* Task priority */
                                    (TaskHandle_t*  )&MUSICTask_Handler,    /* Task handle */
                                    (BaseType_t     ) 0);                   /* Core on which the task runs */
        }
    }
}

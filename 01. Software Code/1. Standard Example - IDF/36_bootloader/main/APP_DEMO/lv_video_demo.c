/**
 ****************************************************************************************************
 * @file        lv_video.c
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-11-04
 * @brief       Video player
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


static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;
AVI_INFO g_avix;                                        /* AVI file-related information */
char *const AVI_VIDS_FLAG_TBL[2] = {"00dc", "01dc"};    /* Video encoding flag strings, 00dc/01dc */
char *const AVI_AUDS_FLAG_TBL[2] = {"00wb", "01wb"};    /* Audio encoding flag strings, 00wb/01wb */

uint16_t video_curindex;                                /* Current index */
FILINFO *vfileinfo;                                     /* File information */
uint8_t *video_pname;                                   /* File name with path */
uint16_t totavinum;                                     /* Total number of music files */
uint8_t *framebuf;
uint32_t *voffsettbl;
FF_DIR vdir;
FIL *video_favi;
extern uint8_t frameup;
static lv_obj_t * video_frame;
static lv_obj_t * video_time_slider;
static lv_obj_t * video_now_time_label;
static lv_obj_t * video_total_time_label;
static lv_obj_t *video_obj_cont_4;
static lv_obj_t *video_play_label;
static lv_obj_t *video_obj_cont_5;
static lv_obj_t *video_last_label;
static lv_obj_t *video_obj_cont_6;
static lv_obj_t *video_next_label;
uint8_t video_play_state = 0;
uint8_t video_key = 0;
lv_timer_t * video_timer;
extern uint8_t sd_check_en;                         /* SD card detection flag */
uint8_t exit_flasg = 0;
extern SemaphoreHandle_t BinarySemaphore;          /* Binary semaphore handle */
extern uint8_t decode_en;     /* Can exit only after image decoding */

/* VIDEO task configuration
 * Includes: task handle, task priority, stack size, task creation
 */
#define VIDEO_PRIO      30                              /* Task priority */
#define VIDEO_STK_SIZE  5 * 1024                        /* Task stack size */
TaskHandle_t            VIDEOTask_Handler;              /* Task handle */
void video(void *pvParameters);                         /* Task function */

/**
 * @brief  Video playback event callback
 * @param  *e : collection of event-related parameters containing all event data
 * @return none
 */
static void video_play_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);                                         /* Get the trigger source */
    lv_event_code_t code = lv_event_get_code(e);                                       /* Get the event type */
    
    if (target == video_obj_cont_4)        /* Start, stop */
    {
        if (code == LV_EVENT_CLICKED)
        {
            if (video_play_state ==0)
            {
                video_key = VIDEO_PAUSE;
                lv_label_set_text(video_play_label, LV_SYMBOL_PLAY);
                video_play_state = 1;
            }
            else
            {
                video_key = VIDEO_PLAY;
                lv_label_set_text(video_play_label, LV_SYMBOL_PAUSE);
                video_play_state = 0;
            }
        }
    }
    else if (target == video_obj_cont_5)       /* Next song */
    {
        if (code == LV_EVENT_PRESSED)
        {
            lv_obj_set_style_text_color(video_last_label, lv_color_hex(0x272727), 0);
        }
        else if (code == LV_EVENT_RELEASED)
        {
            video_key = VIDEO_NEXT;
            lv_obj_set_style_text_color(video_last_label, lv_color_hex(0xffffff), 0);
            lv_label_set_text(video_play_label, LV_SYMBOL_PAUSE);
            video_play_state = 0;
        }
    }
    else if (target == video_obj_cont_6)       /* Previous song */
    {
        if (code == LV_EVENT_PRESSED)
        {
            lv_obj_set_style_text_color(video_next_label, lv_color_hex(0x272727), 0);
        }
        else if (code == LV_EVENT_RELEASED)
        {
            video_key = VIDEO_PREV;
            lv_obj_set_style_text_color(video_next_label, lv_color_hex(0xffffff), 0);
            lv_label_set_text(video_play_label, LV_SYMBOL_PAUSE);
            video_play_state = 0;
        }
    }
}

/**
 * @brief       AVI decoding initialization
 * @param       buf  : input buffer
 * @param       size : buffer size
 * @retval      res
 *    @arg      OK, AVI file parsed successfully
 *    @arg      other, error code
 */
AVISTATUS avi_init(uint8_t *buf, uint32_t size)
{
    uint16_t offset;
    uint8_t *tbuf;
    AVISTATUS res = AVI_OK;
    AVI_HEADER *aviheader;
    LIST_HEADER *listheader;
    AVIH_HEADER *avihheader;
    STRH_HEADER *strhheader;

    STRF_BMPHEADER *bmpheader;
    STRF_WAVHEADER *wavheader;

    tbuf = buf;
    aviheader = (AVI_HEADER *)buf;
    if (aviheader->RiffID != AVI_RIFF_ID)
    {
        return AVI_RIFF_ERR;        /* RIFF ID error */
    }

    if (aviheader->AviID != AVI_AVI_ID)
    {
        return AVI_AVI_ERR;         /* AVI ID error */
    }

    buf += sizeof(AVI_HEADER);      /* Offset */
    listheader = (LIST_HEADER *)(buf);
    if (listheader->ListID != AVI_LIST_ID)
    {
        return AVI_LIST_ERR;        /* LIST ID error */
    }

    if (listheader->ListType != AVI_HDRL_ID)
    {
        return AVI_HDRL_ERR;        /* HDRL ID error */
    }

    buf += sizeof(LIST_HEADER);     /* Offset */
    avihheader = (AVIH_HEADER *)(buf);
    if (avihheader->BlockID != AVI_AVIH_ID)
    {
        return AVI_AVIH_ERR;        /* AVIH ID error */
    }

    g_avix.SecPerFrame = avihheader->SecPerFrame;   /* Get the frame interval time */
    g_avix.TotalFrame = avihheader->TotalFrame;     /* Get the total number of frames */
    buf += avihheader->BlockSize + 8;               /* Offset */
    listheader = (LIST_HEADER *)(buf);
    if (listheader->ListID != AVI_LIST_ID)
    {
        return AVI_LIST_ERR;        /* LIST ID error */
    }

    if (listheader->ListType != AVI_STRL_ID)
    {
        return AVI_STRL_ERR;        /* STRL ID error */
    }

    strhheader = (STRH_HEADER *)(buf + 12);
    if (strhheader->BlockID != AVI_STRH_ID)
    {
        return AVI_STRH_ERR;        /* STRH ID error */
    }

    if (strhheader->StreamType == AVI_VIDS_STREAM)  /* Video frames first */
    {
        if (strhheader->Handler != AVI_FORMAT_MJPG)
        {
            return AVI_FORMAT_ERR;  /* Non-MJPG video stream, unsupported */
        }

        g_avix.VideoFLAG = AVI_VIDS_FLAG_TBL[0];    /* Video stream flag  "00dc" */
        g_avix.AudioFLAG = AVI_AUDS_FLAG_TBL[1];    /* Audio stream flag  "01wb" */
        bmpheader = (STRF_BMPHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
        if (bmpheader->BlockID != AVI_STRF_ID)
        {
            return AVI_STRF_ERR;    /* STRF ID error */
        }

        g_avix.Width = bmpheader->bmiHeader.Width;
        g_avix.Height = bmpheader->bmiHeader.Height;
        buf += listheader->BlockSize + 8;       /* Offset */
        listheader = (LIST_HEADER *)(buf);
        if (listheader->ListID != AVI_LIST_ID)  /* A video file with no audio frames */
        {
            g_avix.SampleRate = 0;              /* Audio sample rate */
            g_avix.Channels = 0;                /* Number of audio channels */
            g_avix.AudioType = 0;               /* Audio format */

        }
        else
        {
            if (listheader->ListType != AVI_STRL_ID)
            {
                return AVI_STRL_ERR;    /* STRL ID error */
            }

            strhheader = (STRH_HEADER *)(buf + 12);
            if (strhheader->BlockID != AVI_STRH_ID)
            {
                return AVI_STRH_ERR;    /* STRH ID error */
            }

            if (strhheader->StreamType != AVI_AUDS_STREAM)
            {
                return AVI_FORMAT_ERR;  /* Format error */
            }

            wavheader = (STRF_WAVHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
            if (wavheader->BlockID != AVI_STRF_ID)
            {
                return AVI_STRF_ERR;    /* STRF ID error */
            }

            g_avix.SampleRate = wavheader->SampleRate;      /* Audio sample rate */
            g_avix.Channels = wavheader->Channels;          /* Number of audio channels */
            g_avix.AudioType = wavheader->FormatTag;        /* Audio format */
        }
    }
    else if (strhheader->StreamType == AVI_AUDS_STREAM)     /* Audio frames first */
    { 
        g_avix.VideoFLAG = AVI_VIDS_FLAG_TBL[1];            /* Video stream flag  "01dc" */
        g_avix.AudioFLAG = AVI_AUDS_FLAG_TBL[0];            /* Audio stream flag  "00wb" */
        wavheader = (STRF_WAVHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
        if (wavheader->BlockID != AVI_STRF_ID)
        {
            return AVI_STRF_ERR;                            /* STRF ID error */
        }
 
        g_avix.SampleRate = wavheader->SampleRate;          /* Audio sample rate */
        g_avix.Channels = wavheader->Channels;              /* Number of audio channels */
        g_avix.AudioType = wavheader->FormatTag;            /* Audio format */
        buf += listheader->BlockSize + 8;                   /* Offset */
        listheader = (LIST_HEADER *)(buf);
        if (listheader->ListID != AVI_LIST_ID)
        {
            return AVI_LIST_ERR;    /* LIST ID error */
        }

        if (listheader->ListType != AVI_STRL_ID)
        {
            return AVI_STRL_ERR;    /* STRL ID error */
        }

        strhheader = (STRH_HEADER *)(buf + 12);
        if (strhheader->BlockID != AVI_STRH_ID)
        {
            return AVI_STRH_ERR;    /* STRH ID error */
        }

        if (strhheader->StreamType != AVI_VIDS_STREAM)
        {
            return AVI_FORMAT_ERR;  /* Format error */
        }

        bmpheader = (STRF_BMPHEADER *)(buf + 12 + strhheader->BlockSize + 8);   /* strf */
        if (bmpheader->BlockID != AVI_STRF_ID)
        {
            return AVI_STRF_ERR;    /* STRF ID error */
        }

        if (bmpheader->bmiHeader.Compression != AVI_FORMAT_MJPG)
        {
            return AVI_FORMAT_ERR;  /* Format error */
        }

        g_avix.Width = bmpheader->bmiHeader.Width;
        g_avix.Height = bmpheader->bmiHeader.Height;
    }

    offset = avi_srarch_id(tbuf, size, "movi");     /* Find the movi ID */
    if (offset == 0)
    {
        return AVI_MOVI_ERR;        /* MOVI ID error */
    }

    if (g_avix.SampleRate)          /* Only search if there is an audio stream */
    {
        tbuf += offset;
        offset = avi_srarch_id(tbuf, size, g_avix.AudioFLAG);   /* Find the audio stream flag */
        if (offset == 0)
        {
            return AVI_STREAM_ERR;  /* Stream error */
        }
        tbuf += offset + 4;
        g_avix.AudioBufSize = *((uint16_t *)tbuf);              /* Get the audio stream buffer size. */
    }

    printf("avi init ok\r\n");
    printf("g_avix.SecPerFrame:%ld\r\n", g_avix.SecPerFrame);
    printf("g_avix.TotalFrame:%ld\r\n", g_avix.TotalFrame);
    printf("g_avix.Width:%ld\r\n", g_avix.Width);
    printf("g_avix.Height:%ld\r\n", g_avix.Height);
    printf("g_avix.AudioType:%d\r\n", g_avix.AudioType);
    printf("g_avix.SampleRate:%ld\r\n", g_avix.SampleRate);
    printf("g_avix.Channels:%d\r\n", g_avix.Channels);
    printf("g_avix.AudioBufSize:%d\r\n", g_avix.AudioBufSize);
    printf("g_avix.VideoFLAG:%s\r\n", g_avix.VideoFLAG);
    printf("g_avix.AudioFLAG:%s\r\n", g_avix.AudioFLAG);

    return res;
}

/**
 * @brief       Find an ID
 * @param       buf  : input buffer
 * @param       size : buffer size
 * @param       id   : ID to find, must be 4 bytes long
 * @retval      Execution result
 *   @arg       0     , not found
 *   @arg       other  , movi ID offset
 */
uint32_t avi_srarch_id(uint8_t *buf, uint32_t size, char *id)
{
    uint32_t i;
    uint32_t idsize = 0;
    size -= 4;
    for (i = 0; i < size; i++)
    {
        if ((buf[i] == id[0]) &&
            (buf[i + 1] == id[1]) &&
            (buf[i + 2] == id[2]) &&
            (buf[i + 3] == id[3]))
        {
            idsize = MAKEDWORD(buf + i + 4);    /* Get the frame size; it must be greater than 16 bytes to return, otherwise it is not valid data */

            if (idsize > 0X10)return i;         /* Find the position of "id" */
        }
    }

    return 0;
}

/**
 * @brief       Get stream information
 * @param       buf  : stream start address (must start with 01wb/00wb/01dc/00dc)
 * @retval      Execution result
 *   @arg       AVI_OK, AVI file parsed successfully
 *   @arg       other  , error code
 */
AVISTATUS avi_get_streaminfo(uint8_t *buf)
{
    g_avix.StreamID = MAKEWORD(buf + 2);    /* Get the stream type */
    g_avix.StreamSize = MAKEDWORD(buf + 4); /* Get the stream size */

    if (g_avix.StreamSize > AVI_MAX_FRAME_SIZE)   /* Frame size too large, return an error directly */
    {
        printf("FRAME SIZE OVER:%ld\r\n", g_avix.StreamSize);
        g_avix.StreamSize = 0;
        return AVI_STREAM_ERR;
    }
    
    if (g_avix.StreamSize % 2)
    {
        g_avix.StreamSize++;    /* Add 1 for odd values (g_avix.StreamSize must be even) */
    }

    if (g_avix.StreamID == AVI_VIDS_FLAG || g_avix.StreamID == AVI_AUDS_FLAG)
    {
        return AVI_OK;
    }

    return AVI_STREAM_ERR;
}

/**
 * @brief       Display the current playback time
 * @param       favi   : currently playing video file
 * @param       aviinfo: AVI control structure
 * @retval      none
 */
void video_time_show(FIL *favi, AVI_INFO *aviinfo)
{
    static uint32_t oldsec;                                         /* Previous playback time */
    
    uint32_t totsec = 0;                                            /* Total video file time */
    uint32_t cursec;                                                /* Current playback time */
    
    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;   /* Total song length (unit: ms) */
    totsec /= 1000;                                                 /* Number of seconds. */
    cursec = ((double)favi->fptr / favi->obj.objsize) * totsec;     /* Get the current playback second */
    
    if (oldsec != cursec)                                           /* Display time needs updating */
    {
        oldsec = cursec;
        lv_slider_set_range(video_time_slider,0,totsec);
        lv_slider_set_value(video_time_slider,cursec,LV_ANIM_ON);
        lv_label_set_text_fmt(video_now_time_label, "%02ld:%02ld:%02ld", cursec / 3600, (cursec % 3600) / 60, cursec % 60);  /* Get the current value and update the display */
        lv_label_set_text_fmt(video_total_time_label, "%02ld:%02ld:%02ld", totsec / 3600, (totsec % 3600) / 60, totsec % 60);  /* Get the current value and update the display */
    }
}

extern uint8_t * video_buf;
extern struct jpeg_decompress_struct *cinfo;
extern struct my_error_mgr *jerr;


lv_img_dsc_t video_img_dsc = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = NULL,
};

/**
  * @brief  Delete the video demo
  * @param  none
  * @retval none
  */
void lv_video_del(void)
{
    int i = 10;
    exit_flasg = 1;
    vTaskDelete(VIDEOTask_Handler);
    taskYIELD();
    VIDEOTask_Handler = NULL;
    esp_timer_delete(esp_tim_handle);
    mjpegdec_free();                                                /* Free memory */

    if ((framebuf != NULL))
    {
        free(framebuf);
        framebuf = NULL;
    }

    if ((vfileinfo != NULL) || (video_pname != NULL) || (voffsettbl != NULL)) /* Memory allocation error */
    {
        free(vfileinfo);
        free(video_pname);
        free(voffsettbl);
    }

    lv_obj_del(app_obj_general.current_parent);
    app_obj_general.current_parent = NULL;

    lv_app_show();
    xSemaphoreGive(xGuiSemaphore);
}

/**
 * @brief       video displays the video
 * @param       none
 * @retval      none
 */
void video_show(uint32_t w,uint32_t h,uint8_t * video_buf)
{
    video_img_dsc.header.w = w;
    video_img_dsc.header.h = h;
    video_img_dsc.data_size = w * h * 2;
    video_img_dsc.data = (const uint8_t *)video_buf;
    lv_img_set_src(video_frame,&video_img_dsc);
    video_time_show(video_favi, &g_avix);
}

/**
 * @brief       video
 * @param       pvParameters : passed-in parameter (unused)
 * @retval      none
 */
void video(void *pvParameters)
{
    pvParameters = pvParameters;
    uint8_t res = 0;
    uint8_t key = 0;
    uint8_t temp = 0;
    uint8_t *pbuf = NULL;
    uint32_t nr = 0;
    uint16_t offset = 0;

    res = (uint8_t)f_opendir(&vdir, "0:/VIDEO");                                /* Open the directory */

    if (res == FR_OK)
    {
        video_curindex = 0;

        while (1)
        {
            temp = vdir.dptr;                                                   /* Record the current dptr offset */
            res = (uint8_t)f_readdir(&vdir, vfileinfo);                         /* Read the next file */
            
            if ((res != 0) || (vfileinfo->fname[0] == 0))                       /* On error or end, exit */
            {
                break;
            }
            
            res = exfuns_file_type(vfileinfo->fname);
            
            if ((res & 0xF0) == 0x60)                                           /* It is a video file */
            {
                voffsettbl[video_curindex] = temp;                                  /* Record the index */
                video_curindex++;
            }
        }

        video_curindex = 0;
        res = (uint8_t)f_opendir(&vdir, "0:/VIDEO");                                /* Open the directory */

        while (1)
        {
            dir_sdi(&vdir, voffsettbl[video_curindex]);                             /* Change the current directory index */
            res = (uint8_t)f_readdir(&vdir, vfileinfo);                             /* Read the next file in the directory */
            
            if ((res != 0) || (vfileinfo->fname[0] == 0))                           /* On error or end, exit */
            {
                printf("err\r\n");
                break;
            }
            
            strcpy((char *)video_pname, "0:/VIDEO/");                               /* Copy the path (directory) */
            strcat((char *)video_pname, (const char *)vfileinfo->fname);            /* Append the file name */
            memset(framebuf, 0, AVI_MAX_FRAME_SIZE);
            
            res = (uint8_t)f_open(video_favi, (const TCHAR *)video_pname, FA_READ);

            if (res == FR_OK)
            {
                pbuf = framebuf;
                res = (uint8_t)f_read(video_favi, pbuf, AVI_MAX_FRAME_SIZE, (UINT*)&nr);  /* Start reading */

                if (res != 0)
                {
                    printf("fread error:%d\r\n", res);
                    break;
                }

                res = avi_init(pbuf, AVI_MAX_FRAME_SIZE);                           /* AVI parsing */

                if (res != 0)
                {
                    printf("avi error:%d\r\n", res);
                    break;
                }

                esptim_int_init(g_avix.SecPerFrame / 1000, 1000);
                offset = avi_srarch_id(pbuf, AVI_MAX_FRAME_SIZE, "movi");   /* Find the movi ID */
                avi_get_streaminfo(pbuf + offset + 4);                      /* Get stream information */
                f_lseek(video_favi, offset + 12);                           /* Skip the flag ID and seek to the start of the stream data */
                res = mjpegdec_init(0,0);                                   /* Initialize JPG decoding */

                if (res != 0)
                {
                    printf("mjpegdec Fail\r\n");
                    break;
                }

                if (Windows_Width * Windows_Height == g_avix.Width * g_avix.Height)
                {
                    continue;
                }
                else
                {
                    /* Define the image width and height */
                    Windows_Width = g_avix.Width;
                    Windows_Height = g_avix.Height;

                    if (video_buf == NULL)
                    {
                        mjpegdec_malloc();
                    }
                }

                if (g_avix.SampleRate)                                                      /* Initialize only if there is audio information */
                {

                }

                while (1)
                {
                    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
                    if (g_avix.StreamID == AVI_VIDS_FLAG)                                   /* Video stream */
                    {
                        pbuf = framebuf;
                        f_read(video_favi, pbuf, g_avix.StreamSize + 8, (UINT*)&nr);        /* Read the whole frame + the next frame's stream ID information */
                        res = mjpegdec_decode(pbuf, g_avix.StreamSize,video_show);
                        
                        if (res != 0)
                        {
                            printf("decode error!\r\n");
                        }

                        while (frameup == 0);                                   /* Wait for the playback time to arrive */
                        frameup = 0;
                    }
                    else
                    {
                        f_read(video_favi, framebuf, g_avix.StreamSize + 8, &nr);     /* Fill the psai buffer */
                        pbuf = framebuf;
                    }

                    /* Handle SD card removal during video playback */
                    if (sdmmc_get_status(card) != ESP_OK)
                    {
                        xSemaphoreGive(xGuiSemaphore); /* Release the mutex semaphore */

                        while (1)
                        {
                            back_act_key = KEY1_PRES;
                            vTaskDelay(10);
                        }
                    }

                    if (video_key == VIDEO_NEXT || video_key == VIDEO_PREV)                   /* KEY0/KEY2 pressed; play the next/previous video */
                    {
                        key = video_key;
                        video_key = VIDEO_NULL;
                        break;
                    }
                    else if (video_key == VIDEO_PAUSE || video_key == VIDEO_PLAY)
                    {
                        xSemaphoreGive(xGuiSemaphore); /* Release the mutex semaphore */

                        while (1)
                        {
                            if (video_key == VIDEO_PLAY || video_key == VIDEO_NEXT || video_key == VIDEO_PREV)
                            {
                                break;
                            }
                            /* Handle SD card removal while the video is paused */
                            if (sdmmc_get_status(card) != ESP_OK)
                            {
                                xSemaphoreGive(xGuiSemaphore); /* Release the mutex semaphore */

                                while (1)
                                {
                                    back_act_key = KEY1_PRES;
                                    vTaskDelay(10);
                                }
                            }

                            vTaskDelay(10);
                        }

                        xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
                    }

                    if (avi_get_streaminfo(pbuf + g_avix.StreamSize) != 0)      /* Read the next frame stream flag */
                    {
                        key = VIDEO_NEXT;
                        video_key = VIDEO_NULL;
                        break;
                    }

                    xSemaphoreGive(xGuiSemaphore); /* Release the mutex semaphore */
                    vTaskDelay(pdMS_TO_TICKS(5));                               /* Delay 5 ms */
                }

                if (key == VIDEO_PREV)                                   /* Previous song */
                {
                    if (video_curindex)
                    {
                        video_curindex--;
                    }
                    else
                    {
                        video_curindex = totavinum - 1;
                    }
                }
                else if (key == VIDEO_NEXT) /* Next song */
                {
                    video_curindex++;

                    if (video_curindex >= totavinum)
                    {
                        video_curindex = 0;                                   /* At the end, automatically restart from the beginning */
                    }

                }

                esp_timer_delete(esp_tim_handle);
                mjpegdec_free();                                                /* Free memory */
                f_close(video_favi);                                            /* Close the file */
                xSemaphoreGive(xGuiSemaphore); /* Release the mutex semaphore */
            }
        }
    }
}

/**
 * @brief       Get the number of valid video files under the specified path
 * @param       path: specified path
 * @retval      Number of valid video files
 */
static uint16_t video_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;
    FILINFO *tfileinfo;
    
    tfileinfo = (FILINFO *)malloc(sizeof(FILINFO));             /* Allocate memory */
    res = (uint8_t)f_opendir(&tdir, (const TCHAR *)path);       /* Open the directory */
    
    if ((res == 0) && tfileinfo)
    {
        while (1)                                               /* Query the total number of valid files */
        {
            res = (uint8_t)f_readdir(&tdir, tfileinfo);         /* Read one file in the directory */
            
            if ((res != 0) || (tfileinfo->fname[0] == 0))       /* On error or end, exit */
            {
                break;
            }
            
            res = exfuns_file_type(tfileinfo->fname);
            
            if ((res & 0xF0) == 0x60)                           /* It is a video file */
            {
                rval++;
            }
        }
    }
    
    free(tfileinfo);                                            /* Free memory */
    
    return rval;
}

/**
 * @brief  Video demo
 * @param  none
 * @return none
 */
void lv_video_demo(void)
{
    video_curindex = 0;                                /* Current index */
    vfileinfo = 0;                                     /* File information */
    video_pname = 0;                                   /* File name with path */
    totavinum = 0;                                     /* Total number of music files */
    framebuf = NULL;
    voffsettbl = 0;
    video_favi = NULL;
    video_key = VIDEO_NULL;
    video_play_state = 0;
    exit_flasg = 0;

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

        if (f_opendir(&vdir, "0:/VIDEO"))                       /* Open the music folder */
        {
            lv_msgbox("MUSIC folder error");
            return ;
        }
        
        totavinum = video_get_tnum("0:/VIDEO");                 /* Get the total number of valid files */

        if (totavinum == 0)
        {
            lv_msgbox("No music files");
            return ;
        }

        vfileinfo = (FILINFO*)malloc(sizeof(FILINFO));          /* Allocate memory */
        video_pname = (uint8_t *)malloc(255 * 2 + 1);           /* Allocate memory for the file name with path */
        voffsettbl = (uint32_t *)malloc(4 * totavinum);         /* Allocate 4*totavinum bytes to store the music file offset block index */

        if ((vfileinfo == NULL) || (video_pname == NULL) || (voffsettbl == NULL)) /* Memory allocation error */
        {
            lv_msgbox("memory allocation failed");
            return ;
        }

        framebuf = (uint8_t *)malloc(AVI_MAX_FRAME_SIZE);       /* Allocate memory */
        video_favi = (FIL *)malloc(sizeof(FIL));

        if ((framebuf == NULL) || (video_favi == NULL))
        {
            lv_msgbox("memory error!");
            return ;
        }

        memset(framebuf, 0, AVI_MAX_FRAME_SIZE);
        memset(video_pname, 0, 255 * 2 + 1);
        memset(voffsettbl, 0, 4 * totavinum);

        lv_app_del();
        /* Create the main container of this screen */
        lv_obj_t *video_obj = lv_obj_create(lv_scr_act());
        lv_obj_set_style_radius(video_obj, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(video_obj, lv_color_make(0,0,0), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(video_obj,LV_OPA_100,LV_STATE_DEFAULT);
        lv_obj_set_size(video_obj,lv_obj_get_width(lv_scr_act()),lv_obj_get_height(lv_scr_act()));
        lv_obj_set_style_border_opa(video_obj,LV_OPA_0,LV_STATE_DEFAULT);
        lv_obj_set_pos(video_obj,0,0);
        lv_obj_clear_flag(video_obj, LV_OBJ_FLAG_SCROLLABLE);

        app_obj_general.current_parent = video_obj;                 /* Point to the current screen container */
        app_obj_general.Function = lv_video_del;                    /* Delete this screen */

        video_frame = lv_img_create(video_obj);
        lv_obj_remove_style_all(video_frame);
        lv_obj_set_style_outline_color(video_frame,lv_color_hex(0xC0C0C0),LV_STATE_DEFAULT);
        lv_obj_set_style_outline_width(video_frame,2,LV_STATE_DEFAULT);
        lv_obj_align(video_frame,LV_ALIGN_TOP_MID,0,21);

        /* Time bar */
        video_time_slider = lv_slider_create(video_obj);
        lv_obj_set_size(video_time_slider, lv_obj_get_width(lv_scr_act()) - 20, 5);
        lv_slider_set_range(video_time_slider,0,100);
        lv_obj_align_to(video_time_slider, video_frame, LV_ALIGN_OUT_BOTTOM_MID, 0, 200);
        lv_obj_set_style_bg_color(video_time_slider, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(video_time_slider, 255, 0);
        lv_obj_set_style_bg_color(video_time_slider, lv_color_make(192,192,192), LV_STATE_DEFAULT);
        lv_obj_remove_style(video_time_slider, NULL, LV_PART_KNOB);

        /* Elapsed time label */
        video_now_time_label = lv_label_create(video_obj);
        lv_obj_set_style_text_font(video_now_time_label, &lv_font_montserrat_14, 0);                          /* Set font */
        lv_label_set_text(video_now_time_label, "0:00");
        lv_obj_set_style_text_color(video_now_time_label, lv_color_hex(0xffffff), 0);
        lv_obj_align_to(video_now_time_label, video_time_slider, LV_ALIGN_OUT_TOP_LEFT, 0, -10);
        
        /* Total time label */
        video_total_time_label = lv_label_create(video_obj);
        lv_obj_set_style_text_font(video_total_time_label, &lv_font_montserrat_14, 0);                          /* Set font */
        lv_label_set_text(video_total_time_label, "0:00");
        lv_obj_set_style_text_color(video_total_time_label, lv_color_hex(0xffffff), 0);
        lv_obj_align_to(video_total_time_label, video_time_slider, LV_ALIGN_OUT_TOP_RIGHT, -20, -10);

        /****************************** Child container 4 ******************************/
        
        video_obj_cont_4 = lv_obj_create(video_obj);                                            /* Create child container */
        lv_obj_remove_style_all(video_obj_cont_4);
        lv_obj_set_size(video_obj_cont_4, 70, 70);                                              /* Set size */
        lv_obj_set_style_bg_color(video_obj_cont_4, lv_color_make(58,58,70), LV_PART_MAIN);      /* Set background color */
        lv_obj_align(video_obj_cont_4, LV_ALIGN_BOTTOM_MID, 0, -10);                            /* Set position */
        lv_obj_set_style_border_color(video_obj_cont_4,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(video_obj_cont_4,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_border_width(video_obj_cont_4,2,LV_STATE_DEFAULT);
        lv_obj_set_style_translate_y(video_obj_cont_4, 5, LV_STATE_PRESSED);
        lv_obj_set_style_shadow_color(video_obj_cont_4,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_shadow_width(video_obj_cont_4,10,LV_STATE_FOCUS_KEY);
        lv_obj_set_style_radius(video_obj_cont_4, 70, 0);                                       /* Set the corner radius */
        lv_obj_add_event_cb(video_obj_cont_4, video_play_event_cb, LV_EVENT_CLICKED, NULL);
        
        /* Play/pause label */
        video_play_label = lv_label_create(video_obj_cont_4);
        lv_obj_set_style_text_font(video_play_label, &lv_font_montserrat_14, 0);                /* Set font */
        lv_label_set_text(video_play_label, LV_SYMBOL_PAUSE);
        lv_obj_set_style_text_color(video_play_label, lv_color_hex(0xffffff), 0);
        lv_obj_align(video_play_label, LV_ALIGN_CENTER, 0, 0);

        /****************************** Child container 5 ******************************/
        
        video_obj_cont_5 = lv_obj_create(video_obj);                                            /* Create child container */
        lv_obj_remove_style_all(video_obj_cont_5);
        lv_obj_set_size(video_obj_cont_5, 50, 50);                                              /* Set size */
        lv_obj_set_style_border_color(video_obj_cont_5,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(video_obj_cont_5,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_align_to(video_obj_cont_5, video_obj_cont_4, LV_ALIGN_OUT_LEFT_MID, -20, 0);
        lv_obj_set_style_border_width(video_obj_cont_5,2,LV_STATE_DEFAULT);
        lv_obj_set_style_translate_y(video_obj_cont_5, 5, LV_STATE_PRESSED);
        lv_obj_set_style_shadow_color(video_obj_cont_5,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_shadow_width(video_obj_cont_5,10,LV_STATE_FOCUS_KEY);
        lv_obj_set_style_radius(video_obj_cont_5, 50, 0);                                       /* Set the corner radius */
        lv_obj_add_event_cb(video_obj_cont_5, video_play_event_cb, LV_EVENT_ALL, NULL);
        
        /* Previous song label */
        video_last_label = lv_label_create(video_obj_cont_5);
        lv_obj_set_style_text_font(video_last_label, &lv_font_montserrat_14, 0);                /* Set font */
        lv_label_set_text(video_last_label, LV_SYMBOL_PREV);
        lv_obj_set_style_text_color(video_last_label, lv_color_hex(0xffffff), 0);
        lv_obj_align(video_last_label, LV_ALIGN_CENTER, 0, 0);

        /****************************** Child container 6 ******************************/
        
        video_obj_cont_6 = lv_obj_create(video_obj);                                            /* Create child container */
        lv_obj_remove_style_all(video_obj_cont_6);
        lv_obj_set_size(video_obj_cont_6, 50, 50);                                              /* Set size */
        lv_obj_set_style_border_color(video_obj_cont_6,lv_color_hex(0xFFFFFF),LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(video_obj_cont_6,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_align_to(video_obj_cont_6, video_obj_cont_4, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
        lv_obj_set_style_border_width(video_obj_cont_6,2,LV_STATE_DEFAULT);
        lv_obj_set_style_translate_y(video_obj_cont_6, 5, LV_STATE_PRESSED);
        lv_obj_set_style_shadow_color(video_obj_cont_6,lv_color_hex(0x1E90FF),LV_STATE_FOCUS_KEY);
        lv_obj_set_style_shadow_width(video_obj_cont_6,10,LV_STATE_FOCUS_KEY);
        lv_obj_set_style_radius(video_obj_cont_6, 50, 0);                                       /* Set the corner radius */
        lv_obj_add_event_cb(video_obj_cont_6, video_play_event_cb, LV_EVENT_ALL, NULL);

        /* Next song label */
        video_next_label = lv_label_create(video_obj_cont_6);
        lv_obj_set_style_text_font(video_next_label, &lv_font_montserrat_14, 0);                /* Set font */
        lv_label_set_text(video_next_label, LV_SYMBOL_NEXT);
        lv_obj_set_style_text_color(video_next_label, lv_color_hex(0xffffff), 0);
        lv_obj_align(video_next_label, LV_ALIGN_CENTER, 0, 0);

        lv_group_add_obj(ctrl_g, video_obj_cont_5);
        lv_group_add_obj(ctrl_g, video_obj_cont_4);
        lv_group_add_obj(ctrl_g, video_obj_cont_6);
        lv_group_remove_obj(video_time_slider);
        lv_group_focus_obj(video_obj_cont_5);                               /* Focus */

        if (VIDEOTask_Handler == NULL)
        {
            /* Create the VIDEO task */
            xTaskCreatePinnedToCore((TaskFunction_t )video,                 /* Task function */
                                    (const char*    )"video",               /* Task name */
                                    (uint16_t       )VIDEO_STK_SIZE,        /* Task stack size */
                                    (void*          )NULL,                  /* Parameter passed to the task function */
                                    (UBaseType_t    )VIDEO_PRIO,            /* Task priority */
                                    (TaskHandle_t*  )&VIDEOTask_Handler,    /* Task handle */
                                    (BaseType_t     ) 1);                   /* Core on which the task runs */
        }
    }
}

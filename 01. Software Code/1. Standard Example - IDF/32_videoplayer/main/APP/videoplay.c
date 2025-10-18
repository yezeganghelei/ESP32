/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file videoplay.c
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief Video Player Application Code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "videoplay.h"

extern uint8_t frameup;

/**
 * @brief       获取Specify path下有效videodocument的数量
 * @param       path: Specify path
 * @retval      有效videodocument的数量
 */
static uint16_t video_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;
    FILINFO *tfileinfo;
    
    tfileinfo = (FILINFO *)malloc(sizeof(FILINFO));             /* Apply for memory */
    res = (uint8_t)f_opendir(&tdir, (const TCHAR *)path);       /* 打开Table of contents */
    
    if ((res == 0) && tfileinfo)
    {
        while (1)                                               /* 查询总的有效document数 */
        {
            res = (uint8_t)f_readdir(&tdir, tfileinfo);         /* 读取Table of contents下的一个document */
            
            if ((res != 0) || (tfileinfo->fname[0] == 0))       /* Error or to the end，quit */
            {
                break;
            }
            
            res = exfuns_file_type(tfileinfo->fname);
            
            if ((res & 0xF0) == 0x60)                           /* 是videodocument */
            {
                rval++;
            }
        }
    }
    
    free(tfileinfo);                                            /* Free memory */
    
    return rval;
}

/**
 * @brief       Display basic video information
 * @param       name : video名字
 * @param       index: Current index
 * @param       total: 总document数
 * @retval      none
 */
static void video_bmsg_show(uint8_t *name, uint16_t index, uint16_t total)
{
    uint8_t *buf;
    
    buf = malloc(100);                                                /* Apply for 100 bytes of memory */
    
    sprintf((char *)buf, "document名:%s", name);
//    text_show_string(10, 10, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);   /* 显示document名 */
    
    sprintf((char *)buf, "index:%d/%d", index, total);
//    text_show_string(10, 30, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);   /* 显示index */
    
    free(buf);                                                        /* Free memory */
}

/**
 * @brief       显示当前videodocument的相关information
 * @param       aviinfo: aviControl structure
 * @retval      none
 */
static void video_info_show(AVI_INFO *aviinfo)
{
    uint8_t *buf;
    
    buf = malloc(100);                                                /* Apply for 100 bytes of memory */
    
    sprintf((char *)buf, "Number of channels:%d,Sampling rate:%ld", aviinfo->Channels, aviinfo->SampleRate * 10);
    text_show_string(10, 10, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);   /* Show song name */
    
    sprintf((char *)buf, "Frame rate:%ldframe", 1000 / (aviinfo->SecPerFrame / 1000));
    text_show_string(10, 30, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);   /* Show song name */
    
    free(buf);                                                        /* Free memory */
}

/**
 * @brief       显示Current playback time
 * @param       favi   : 当前Play的videodocument
 * @param       aviinfo: aviControl structure
 * @retval      none
 */
void video_time_show(FIL *favi, AVI_INFO *aviinfo)
{
    static uint32_t oldsec;                                         /* Last playback time */
    
    uint8_t *buf;
    
    uint32_t totsec = 0;                                            /* videodocument总时间 */
    uint32_t cursec;                                                /* Current playback time */
    
    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;   /* Total song length(unit:ms) */
    totsec /= 1000;                                                 /* Number of seconds. */
    cursec = ((double)favi->fptr / favi->obj.objsize) * totsec;     /* Get the current number of seconds played */
    
    if (oldsec != cursec)                                           /* Need to update the display time */
    {
        buf = malloc(100);                                          /* Apply for 100 bytes of memory */
        oldsec = cursec;
        
        sprintf((char *)buf, "Play time:%02ld:%02ld:%02ld/%02ld:%02ld:%02ld", cursec / 3600, (cursec % 3600) / 60, cursec % 60, totsec / 3600, (totsec % 3600) / 60, totsec % 60);
        text_show_string(10, 50, lcd_self.width - 10, 16, (char *)buf, 16, 0, RED);   /* Show song name */
        
        free(buf);
    }
}

/**
 * @brief       PlayMJPEGvideo
 * @param       pname: videodocument名
 * @retval      Key key value
 *              KEY0_PRES: Previous video
 *              WKUP_PRES: 下一个video
 *              Other values   : Error code
 */
static uint8_t video_play_mjpeg(uint8_t *pname)
{
    uint8_t *framebuf;
    uint8_t *pbuf;
    uint8_t res = 0;
    uint16_t offset;
    uint32_t nr;
    uint8_t key;
    FIL *favi;

    framebuf = (uint8_t *)malloc(AVI_VIDEO_BUF_SIZE);   /* Apply for memory */
    favi = (FIL *)malloc(sizeof(FIL));

    if ((framebuf == NULL) || (favi == NULL))
    {
        printf("memory error!\r\n");
        res = 0xFF;
    }

    memset(framebuf, 0, AVI_VIDEO_BUF_SIZE);
    
    while (res == 0)
    {
        res = (uint8_t)f_open(favi, (const TCHAR *)pname, FA_READ);                                                     /* 打开document */
        
        if (res == 0)
        {
            pbuf = framebuf;
            res = (uint8_t)f_read(favi, pbuf, AVI_VIDEO_BUF_SIZE, (UINT*)&nr);                                                 /* Start reading */
            
            if (res != 0)
            {
                printf("fread error:%d\r\n", res);
                break;
            }
            
            res = avi_init(pbuf, AVI_VIDEO_BUF_SIZE);                                                                   /* AVIAnalysis */
            
            if (res != 0)
            {
                printf("avi error:%d\r\n", res);
                break;
            }
            
            video_info_show(&g_avix);
            esptim_int_init(g_avix.SecPerFrame / 1000, 1000);
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");                                                   /* Looking for movie ID */
            avi_get_streaminfo(pbuf + offset + 4);                                                                      /* 获取流information */
            f_lseek(favi, offset + 12);                                                                                 /* Skip signID，读地址Offset到流数据start处 */
            res = mjpegdec_init((lcd_self.width - g_avix.Width) / 2, 110 + (lcd_self.height - 110 - g_avix.Height) / 2);/* initializationJPGdecoding */
            /* Define the width and height of an image */
            Windows_Width = g_avix.Width;
            Windows_Height = g_avix.Height;

            if (g_avix.SampleRate)                                          /* 有音频information,才initialization */
            {
                printf("g_avix.SampleRate:%ld\r\n",g_avix.SampleRate);
                es8388_sai_cfg(0, 3);                                       /* Philips standard, 16-bit data length */
                i2s_set_samplerate_bits_sample(g_avix.SampleRate,I2S_BITS_PER_SAMPLE_16BIT);    /* 设置Sampling rate */
                i2s_start(I2S_NUM);
            }

            while (1)
            {
                if (g_avix.StreamID == AVI_VIDS_FLAG)                       /* video流 */
                {
                    pbuf = framebuf;
                    f_read(favi, pbuf, g_avix.StreamSize + 8, (UINT*)&nr);  /* 读取整frame+Next frame data streamIDinformation */
                    res = mjpegdec_decode(pbuf, g_avix.StreamSize);
                    
                    if (res != 0)
                    {
                        printf("decode error!\r\n");
                    }

                    while (frameup == 0);                                   /* 等待Play时间到达 */
                    frameup = 0;
                }
                else
                {
                    video_time_show(favi, &g_avix);                         /* 显示Current playback time */
                    f_read(favi, framebuf, g_avix.StreamSize + 8, &nr);     /* fill psaibuf */
                    pbuf = framebuf;
                    i2s_tx_write(framebuf, g_avix.StreamSize);          /* Data conversion+Send toDAC */
                }
                
                key = xl9555_key_scan(0);

                if (key == KEY0_PRES || key == KEY2_PRES)                   /* KEY0/KEY2Press,Play the next one/Previous video */
                {
                    res = key;
                    break;
                }
                else if (key == KEY1_PRES || key == KEY3_PRES)
                {
                    i2s_stop(I2S_NUM);                                      /* Turn off the audio */
                    video_seek(favi, &g_avix, framebuf);
                    pbuf = framebuf;
                    i2s_start(I2S_NUM);                                     /* Turn on DMA playback */
                }

                if (avi_get_streaminfo(pbuf + g_avix.StreamSize) != 0)      /* 读取下一frame流标志 */
                {
                    printf("g_frame error\r\n");
                    res = KEY0_PRES;
                    break;
                }
            }

            i2s_stop(I2S_NUM);
            esp_timer_stop(esp_tim_handle);
            lcd_set_window(0, 0, lcd_self.width, lcd_self.height);          /* Recovery window */
            mjpegdec_free();                                                /* Free memory */
            f_close(favi);                                                  /* 关闭document */
        }
    }

    i2s_zero_dma_buffer(I2S_NUM);
    free(framebuf);
    free(favi);
    
    return res;
}

/**
 * @brief play video
 * @param None
 * @retval None
 */
void video_play(void)
{
    uint8_t res;
    FF_DIR vdir;
    FILINFO *vfileinfo;
    uint8_t *pname;
    uint16_t totavinum;
    uint16_t curindex;
    uint32_t *voffsettbl;
    uint8_t key;
    uint32_t temp;
    
    while (f_opendir(&vdir, "0:/VIDEO") != FR_OK)                               /* examineVIDEOdocument夹是否存在 */
    {
        text_show_string(60, 190, 240, 16, "VIDEOdocument夹mistake!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(60, 190, 240, 206, WHITE);
        vTaskDelay(200);
    }
    
    totavinum = video_get_tnum("0:/VIDEO");                                     /* examine是否有videodocument */
    
    while (totavinum == 0)
    {
        text_show_string(60, 190, 240, 16, "没有videodocument!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(60, 190, 240, 146, WHITE);
        vTaskDelay(200);
    }
    
    vfileinfo = (FILINFO *)malloc(sizeof(FILINFO));                             /* Apply for memory */
    pname = (uint8_t *)malloc(2 * 255 + 1);
    voffsettbl = (uint32_t *)malloc(totavinum  * 4);
    
    while ((vfileinfo == NULL) || (pname == NULL) || (voffsettbl == NULL))
    {
        text_show_string(60, 190, 240, 16, "Memory allocation failed!", 16, 0, RED);
        vTaskDelay(200);
        lcd_fill(60, 190, 240, 146, WHITE);
        vTaskDelay(200);
    }
    
    res = (uint8_t)f_opendir(&vdir, "0:/VIDEO");                                /* 打开Table of contents */
    
    if (res == 0)
    {
        curindex = 0;
        
        while (1)
        {
            temp = vdir.dptr;                                                   /* Record the current dptr offset */
            res = (uint8_t)f_readdir(&vdir, vfileinfo);                         /* 读取下一个document */
            
            if ((res != 0) || (vfileinfo->fname[0] == 0))                       /* Error or to the end，quit */
            {
                break;
            }
            
            res = exfuns_file_type(vfileinfo->fname);
            
            if ((res & 0xF0) == 0x60)                                           /* 是videodocument */
            {
                voffsettbl[curindex] = temp;                                    /* Record index */
                curindex++;
            }
        }
    }
    
    curindex = 0;
    res = (uint8_t)f_opendir(&vdir, "0:/VIDEO");                                /* 打开Table of contents */

    while (res == 0)
    {
        dir_sdi(&vdir, voffsettbl[curindex]);                                   /* 改变当前Table of contentsindex */
        res = (uint8_t)f_readdir(&vdir, vfileinfo);                             /* 读取Table of contents的下一个document */
        
        if ((res != 0) || (vfileinfo->fname[0] == 0))                           /* Error or to the end，quit */
        {
            break;
        }
        
        strcpy((char *)pname, "0:/VIDEO/");                                     /* Copy path (directory) */
        strcat((char *)pname, (const char *)vfileinfo->fname);                  /* 将document名接在后面 */
        lcd_clear(WHITE);
        video_bmsg_show((uint8_t *)vfileinfo->fname, curindex + 1, totavinum);  /* Display basic video information */
        
        key = video_play_mjpeg(pname);

        if (key == KEY2_PRES)   /* Previous video */
        {
            if (curindex != 0)
            {
                curindex--;
            }
            else
            {
                curindex = totavinum - 1;
            }
        }
        else if (key == KEY0_PRES)  /* 下一个video */
        {
            curindex++;
            if (curindex >= totavinum)
            {
                curindex = 0;
            }
        }
        else
        {
            break;
        }
    }
    
    free(vfileinfo);      /* Free memory */
    free(pname);
    free(voffsettbl);
}

/**
 * @brief       AVIdocument查找
 * @param       favi    : AVIdocument
 * @param       aviinfo : AVIinformation结构体
 * @param       mbuf    : Data buffer
 * @retval      Execution results
 *   @arg       0    , success
 *   @arg       other , mistake
 */
uint8_t video_seek(FIL *favi, AVI_INFO *aviinfo, uint8_t *mbuf)
{
    uint32_t fpos = favi->fptr;
    uint8_t *pbuf;
    uint16_t offset;
    uint32_t br;
    uint32_t delta;
    uint32_t totsec;
    uint8_t key;

    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;
    totsec /= 1000;                             /* Number of seconds */
    delta = (favi->obj.objsize / totsec) * 5;   /* Every time you move forward5The amount of data in seconds */

    while (1)
    {
        key = xl9555_key_scan(1);

        if (key == KEY3_PRES)                   /* fast forward */
        {
            if (fpos < favi->obj.objsize)
            {
                fpos += delta;
            }

            if (fpos > (favi->obj.objsize - AVI_VIDEO_BUF_SIZE))
            {
                fpos = favi->obj.objsize - AVI_VIDEO_BUF_SIZE;
            }
        }
        else if (key == KEY1_PRES)              /* Rewind */
        {
            if (fpos > delta)
            {
                fpos -= delta;
            }
            else
            {
                fpos = 0;
            }
        }
        else 
        {
            break;
        }

        f_lseek(favi, fpos);
        f_read(favi, mbuf, AVI_VIDEO_BUF_SIZE, &br);       /* Read the entire frame+next data streamIDinformation */
        pbuf = mbuf;
        
        if (fpos == 0)                                     /* from0start,得先Looking for movie ID */
        {
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");
        }
        else
        {
            offset = 0;
        }

        offset += avi_srarch_id(pbuf + offset, AVI_VIDEO_BUF_SIZE, g_avix.VideoFLAG); /* Find video frames */
        avi_get_streaminfo(pbuf + offset);                  /* 获取流information */
        f_lseek(favi, fpos + offset + 8);                   /* Skip the flag ID, read address is offset to the beginning of the stream data */
        
        if (g_avix.StreamID == AVI_VIDS_FLAG)
        {
            f_read(favi, mbuf, g_avix.StreamSize + 8, &br); /* Read the entire frame */
            mjpegdec_decode(mbuf, g_avix.StreamSize);       /* Display video frames */
        }
        else
        {
            printf("error flag");
        }

        video_time_show(favi, &g_avix);
    }

    return 0;
}
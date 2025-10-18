/**
 ******************************************************************************************************
 * @file audioplay.c
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief music player Application code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************
 * @attention

 ******************************************************************************************************
 */

#include "audioplay.h"

__audiodev g_audiodev;          /* Music Playback Controller */

/**
 * @brief       Start audio playback
 * @param       none
 * @retval      none
 */
void audio_start(void)
{
    g_audiodev.status = 3 << 0; /* Start playing+Non-suspended */
    i2s_trx_start();
}

/**
 * @brief stops audio playback
 * @param None
 * @retval None
 */
void audio_stop(void)
{
    g_audiodev.status = 0;
    i2s_trx_stop();
}

/**
 * @brief       getpathUnder the path，Total number of target files
 * @param       path : File path
 * @retval      Total number of valid files
 */
uint16_t audio_get_tnum(uint8_t *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;                                                /* temporaryTable of contents */
    FILINFO *tfileinfo;                                         /* Temporary File information */
    
    tfileinfo = (FILINFO*)malloc(sizeof(FILINFO));              /* Apply for memory */
    
    res = f_opendir(&tdir, (const TCHAR*)path);                 /* Open the directory */
    
    if ((res == FR_OK) && tfileinfo)
    {
        while (1)                                               /* Query the total number of valid files */
        {
            res = f_readdir(&tdir, tfileinfo);                  /* Read a file in the directory */
            
            if ((res != FR_OK) || (tfileinfo->fname[0] == 0))
            {
                break;                                          /* mistakeIt's/It's the end,quit */
            }

            res = exfuns_file_type(tfileinfo->fname);
            
            if ((res & 0xF0) == 0x40)                           /* Take the top four,See if it ismusicdocument */
            {
                rval++;                                         /* Increase the number of valid files1 */
            }
        }
    }
    
    free(tfileinfo);                                            /* free memory */
    
    return rval;
}

/**
 * @brief Show track index
 * @param index : Current index
 * @param total : Total number of files
 * @retval None
 */
void audio_index_show(uint16_t index, uint16_t total)
{
    /* Show the current trackindex,andTotal repertoirenumber */
    lcd_show_num(30 + 0, 230, index, 3, 16, RED);   /* index */
    lcd_show_char(30 + 24, 230, '/', 16, 0, RED);
    lcd_show_num(30 + 32, 230, total, 3, 16, RED);  /* Total repertoire */
}

/**
 * @brief       Show playback time,Bit rate information
 * @param       totsec : Total audio file time
 * @param       cursec : Current playback time
 * @param       bitrate: Bit rate(Bit speed)
 * @retval      none
 */
void audio_msg_show(uint32_t totsec, uint32_t cursec, uint32_t bitrate)
{
    static uint16_t playtime = 0xFFFF;                                  /* Put time mark */
    
    if (playtime != cursec)                                             /* Need to update the display time */
    {
        playtime = cursec;
        
        /* Show playback time */
        lcd_show_xnum(30, 210, playtime / 60, 2, 16, 0X80, RED);        /* minute */
        lcd_show_char(30 + 16, 210, ':', 16, 0, RED);
        lcd_show_xnum(30 + 24, 210, playtime % 60, 2, 16, 0X80, RED);   /* Seconds */
        lcd_show_char(30 + 40, 210, '/', 16, 0, RED);
        
        /* Show total time */
        lcd_show_xnum(30 + 48, 210, totsec / 60, 2, 16, 0X80, RED);     /* minute */
        lcd_show_char(30 + 64, 210, ':', 16, 0, RED);
        lcd_show_xnum(30 + 72, 210, totsec % 60, 2, 16, 0X80, RED);     /* Seconds */
        
        /* Display bit rate */
        lcd_show_num(30 + 110, 210, bitrate / 1000, 4, 16, RED);/* Display bit rate */
        lcd_show_string(30 + 110 + 32 , 210, 200, 16, 16, "Kbps", RED);
    }
}

/**
 * @brief       Playmusic
 * @param       none
 * @retval      none
 */
void audio_play(void)
{
    uint8_t res;
    FF_DIR wavdir;                                              /* Table of contents */
    FILINFO *wavfileinfo;                                       /* File information */
    uint8_t *pname;                                             /* File name with path */
    uint16_t totwavnum;                                         /* musicdocument总number */
    uint16_t curindex;                                          /* Current index */
    uint8_t key;                                                /* Key value */
    uint32_t temp;
    uint32_t *wavoffsettbl;                                     /* musicoffsetIndex table */

    es8388_adda_cfg(1, 0);                                      /* Turn on DAC to turn off ADC */
    es8388_output_cfg(1, 1);                                    /* DAC Select Channel 1 Output */

    while (f_opendir(&wavdir, "0:/MUSIC"))                      /* Openmusicdocument夹 */
    {
        text_show_string(30, 190, 240, 16, "MUSICdocument夹mistake!", 16, 0, BLUE);
        vTaskDelay(200);
        lcd_fill(30, 190, 240, 206, WHITE);                     /* Clear the display */
        vTaskDelay(200);
    }

    totwavnum = audio_get_tnum((uint8_t *)"0:/MUSIC");          /* Get the total number of valid files */
    
    while (totwavnum == NULL)                                   /* musicdocument总numberfor0 */
    {
        text_show_string(30, 190, 240, 16, "Nomusicdocument!", 16, 0, BLUE);
        vTaskDelay(200);
        lcd_fill(30, 190, 240, 146, WHITE);                     /* Clear the display */
        vTaskDelay(200);
    }
    
    wavfileinfo = (FILINFO*)malloc(sizeof(FILINFO));            /* Apply for memory */
    pname = malloc(255 * 2 + 1);                                /* forFile name with pathAllocate memory */
    wavoffsettbl = malloc(4 * totwavnum);                       /* Apply4*totwavnumBytes of memory,For storagemusicdocumentoff blockindex */
    
    while (!wavfileinfo || !pname || !wavoffsettbl)             /* Memory allocation error */
    {
        text_show_string(30, 190, 240, 16, "Memory allocation failed!", 16, 0, BLUE);
        vTaskDelay(200);
        lcd_fill(30, 190, 240, 146, WHITE);                     /* Clear the display */
        vTaskDelay(200);
    }
    
    /* Recordindex */
    res = f_opendir(&wavdir, "0:/MUSIC");                       /* Open the directory */
    
    if (res == FR_OK)
    {
        curindex = 0;                                           /* The current index is 0 */
        
        while (1)                                               /* Query all */
        {
            temp = wavdir.dptr;                                 /* Record the current index */

            res = f_readdir(&wavdir, wavfileinfo);              /* Read a file in the directory */
            
            if ((res != FR_OK) || (wavfileinfo->fname[0] == 0))
            {
                break;                                          /* mistakeIt's/It's the end,quit */
            }

            res = exfuns_file_type(wavfileinfo->fname);
            
            if ((res & 0xF0) == 0x40)                           /* Take the top four,See if it ismusicdocument */
            {
                wavoffsettbl[curindex] = temp;                   /* Recordindex */
                curindex++;
            }
        }
    }
    
    curindex = 0;                                               /* from0Start displaying */
    res = f_opendir(&wavdir, (const TCHAR*)"0:/MUSIC");         /* Open the directory */
    
    while (res == FR_OK)                                        /* Open successfully */
    {
        dir_sdi(&wavdir, wavoffsettbl[curindex]);               /* Change the currentTable of contentsindex */
        res = f_readdir(&wavdir, wavfileinfo);                  /* Read a file in the directory */
        
        if ((res != FR_OK) || (wavfileinfo->fname[0] == 0))
        {
            break;                                              /* mistakeIt's/It's the end,quit */
        }
        
        strcpy((char *)pname, "0:/MUSIC/");                     /* Copy path (directory) */
        strcat((char *)pname, (const char *)wavfileinfo->fname);/* Willfile nameConnected behind */
        lcd_fill(30, 190, lcd_self.width - 1, 190 + 16, WHITE); /* Clear the previous display */
        audio_index_show(curindex + 1, totwavnum);
        text_show_string(30, 190, lcd_self.width - 60, 16, (char *)wavfileinfo->fname, 16, 0, BLUE);   /* Show song name */
        key = audio_play_song(pname);                           /* Play this audio file */
        
        if (key == KEY2_PRES)                                   /* Previous song */
        {
            if (curindex)
            {
                curindex--;
            }
            else
            {
                curindex = totwavnum - 1;
            }
        }
        else if (key == KEY0_PRES)                              /* Next song */
        {
            curindex++;

            if (curindex >= totwavnum)
            {
                curindex = 0;                                   /* By the end,automaticfromStart */
            }
        }
        else
        {
            break;                                              /* Generate mistake */
        }
    }

    free(wavfileinfo);                                          /* free memory */
    free(pname);                                                /* free memory */
    free(wavoffsettbl);                                         /* free memory */
}

/**
 * @brief Play an audio file
 * @param fname : File name
 * @retval key value
 * @arg KEY0_PRES , next song.
 * @arg KEY2_PRES , previous song.
 * @arg Other , error
 */
uint8_t audio_play_song(uint8_t *fname)
{
    uint8_t res;  
    
    res = exfuns_file_type((char *)fname); 

    switch (res)
    {
        case T_WAV:
            res = wav_play_song(fname);
            break;
        case T_MP3:
            /* Implement it by yourself */
            break;

        default:            /* Other documents,Automatically jump to the next song */
            printf("can't play:%s\r\n", fname);
            res = KEY0_PRES;
            break;
    }
    return res;
}
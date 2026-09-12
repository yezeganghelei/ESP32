/**
 ****************************************************************************************************
 * @file        lv_pic_demo.h
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-11-04
 * @brief       Photo album
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

#include "lv_pic_demo.h"


LV_FONT_DECLARE(myFont12)       /* Declare the myFont24 font, which is already flashed to SPIFLASH */
FF_DIR picdir;                  /* Image directory */
FILINFO *pic_picfileinfo;       /* File information */
char *pic_pname;                /* File name with path */
uint16_t pic_totpicnum;         /* Total number of image files */
uint16_t pic_curindex = 0;      /* Current image index */
uint32_t *pic_picoffsettbl;     /* Image file offset index table */
uint8_t pic_key = PIC_NULL;
static lv_obj_t *pic_frame;
static lv_obj_t *pic_last_label;
static lv_obj_t *pic_next_label;
static lv_obj_t *pic_name;
extern uint8_t sd_check_en;                 /* SD card detection flag */
extern uint8_t decode_en;

/* PIC task configuration
 * Includes: task handle, task priority, stack size, task creation
 */
#define PIC_PRIO      10                                /* Task priority */
#define PIC_STK_SIZE  5 * 1024                          /* Task stack size */
TaskHandle_t          PICTask_Handler;                  /* Task handle */
void pic(void *pvParameters);                           /* Task function */

/**
 * @brief       Get the total number of target files under the path
 * @param       path : path
 * @retval      Total number of valid files
 */
uint16_t pic_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;                                    /* Temporary directory */
    FILINFO *tfileinfo;                             /* Temporary file information */
    tfileinfo = (FILINFO *)malloc(sizeof(FILINFO)); /* Allocate memory */
    res = f_opendir(&tdir, (const TCHAR *)path);    /* Open the directory */

    if (res == FR_OK && tfileinfo)
    {
        while (1)                                   /* Query the total number of valid files */
        {
            res = f_readdir(&tdir, tfileinfo);      /* Read one file in the directory */

            if (res != FR_OK || tfileinfo->fname[0] == 0)break; /* On error or end, exit */
            res = exfuns_file_type(tfileinfo->fname);

            if ((res & 0X0F) != 0X00)               /* Take the low nibble to check whether it is an image file */
            {
                rval++;                             /* Increment the valid file count */
            }
        }
    }

    free(tfileinfo);                                /* Free memory */
    return rval;
}

lv_img_dsc_t img_pic_dsc = {
    .header.always_zero = 0,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = NULL,
};

/**
 * @brief       PNG/BMP/JPEG/JPG decoding
 * @param       filename: file name
 * @param       width   : width
 * @param       height  : height
 * @retval      none
 */
void lv_pic_png_bmp_jpeg_decode(uint16_t w,uint16_t h,uint8_t * pic_buf)
{
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
    img_pic_dsc.header.w = w;
    img_pic_dsc.header.h = h;
    img_pic_dsc.data_size = w * h * 2;
    img_pic_dsc.data = (const uint8_t *)pic_buf;
    lv_img_set_src(pic_frame,&img_pic_dsc);
    xSemaphoreGive(xGuiSemaphore);                  /* Release the mutex semaphore */
}

/**
 * @brief       pic
 * @param       pvParameters : passed-in parameter (unused)
 * @retval      none
 */
void pic(void *pvParameters)
{
    pvParameters = pvParameters;
    uint8_t res = 0;
    uint16_t temp = 0;

    while(1)
    {
        /* Record the index */
        res = f_opendir(&picdir, "0:/PICTURE");                             /* Open the directory */

        if (res == FR_OK)
        {
            pic_curindex = 0;                                               /* Current index is 0 */

            while (1)                                                       /* Query everything once */
            {
                temp = picdir.dptr;                                         /* Record the current dptr offset */
                res = f_readdir(&picdir, pic_picfileinfo);                  /* Read one file in the directory */
                if (res != FR_OK || pic_picfileinfo->fname[0] == 0)break;   /* On error or end, exit */

                res = exfuns_file_type(pic_picfileinfo->fname);

                if ((res & 0X0F) != 0X00)                                   /* Take the high nibble to check whether it is an image file */
                {
                    pic_picoffsettbl[pic_curindex] = temp;                  /* Record the index */
                    pic_curindex++;
                }
            }
        }

        pic_curindex = 0;                                                   /* Display starting from 0 */
        res = f_opendir(&picdir, (const TCHAR *)"0:/PICTURE");              /* Open the directory */

        while (res == FR_OK)                                                /* Opened successfully */
        {
            dir_sdi(&picdir, pic_picoffsettbl[pic_curindex]);               /* Change the current directory index */
            res = f_readdir(&picdir, pic_picfileinfo);                      /* Read one file in the directory */

            if (res != FR_OK || pic_picfileinfo->fname[0] == 0)break;       /* On error or end, exit */

            strcpy((char *)pic_pname, "0:/PICTURE/");                       /* Copy the path (directory) */
            strcat((char *)pic_pname, (const char *)pic_picfileinfo->fname);/* Append the file name */
            /* Pass the file name */
            temp = exfuns_file_type(pic_pname);                             /* Get the file type */

            decode_en = ESP_FAIL;                                           /* Prepare for decoding */

            switch (temp)
            {
                case T_BMP:
                    bmp_decode(pic_pname,lcd_self.width,lcd_self.height,lv_pic_png_bmp_jpeg_decode);    /* Decode BMP */
                    break;
                case T_JPG:
                case T_JPEG:
                    jpeg_decode(pic_pname,lcd_self.width,lcd_self.height,lv_pic_png_bmp_jpeg_decode);   /* Decode JPG/JPEG */
                    break;
                case T_PNG:
                    png_decode(pic_pname,lcd_self.width,lcd_self.height,lv_pic_png_bmp_jpeg_decode);    /* Decode PNG */
                    break;
                default:
                    pic_key = PIC_NEXT;                                                                 /* Not an image format!!! */
                    break;
            }

            decode_en = ESP_OK;

            lv_label_set_text_fmt(pic_name,"%s",pic_pname);

            while (1)
            {
                xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

                if (sd_check_en == 0)
                {
                    back_act_key = KEY1_PRES;
                    xSemaphoreGive(xGuiSemaphore);                      /* Release the mutex semaphore */
                }

                if (pic_key == PIC_PREV)                                    /* Previous image */
                {
                    if (pic_curindex)
                    {
                        pic_curindex--;
                    }
                    else
                    {
                        pic_curindex = pic_totpicnum - 1;
                    }

                    pic_key = PIC_NULL;
                    break;
                }
                else if (pic_key == PIC_NEXT)                               /* Next image */
                {
                    pic_curindex++;

                    if (pic_curindex >= pic_totpicnum)
                    {
                        pic_curindex = 0;                                   /* At the end, automatically restart from the beginning */
                    }

                    pic_key = PIC_NULL;
                    break;
                }

                xSemaphoreGive(xGuiSemaphore);                              /* Release the mutex semaphore */

                vTaskDelay(10);
            }

            xSemaphoreGive(xGuiSemaphore);                              /* Release the mutex semaphore */
        }
    }
}

/**
  * @brief  Delete the album demo
  * @param  none
  * @retval none
  */
void lv_pic_del(void)
{
    if (PICTask_Handler != NULL)
    {
        vTaskDelete(PICTask_Handler);
        vTaskDelay(10);
    }

    if (pic_picfileinfo || pic_pname || pic_picoffsettbl)           /* Free memory */
    {
        free(pic_picfileinfo);                                      /* Free memory */
        free(pic_pname);                                            /* Free memory */
        free(pic_picoffsettbl);                                     /* Free memory */
    }
    decode_en = ESP_OK;
    
    lv_obj_clean(app_obj_general.current_parent);
    lv_obj_del(app_obj_general.current_parent);
    app_obj_general.current_parent = NULL;
    lv_app_show();
    xSemaphoreGive(xGuiSemaphore);                                  /* Release the mutex semaphore */
    PICTask_Handler = NULL;
}

/**
 * @brief  Album playback event callback
 * @param  *e : collection of event-related parameters containing all event data
 * @return none
 */
static void pic_play_event_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);      /* Get the trigger source */
    lv_event_code_t code = lv_event_get_code(e);    /* Get the event type */
    
    if (target == pic_last_label)                   /* Next image */
    {
        if (code == LV_EVENT_PRESSED)
        {
            lv_obj_set_style_text_color(pic_last_label, lv_color_hex(0x272727), 0);
        }
        else if (code == LV_EVENT_RELEASED)
        {
            pic_key = PIC_PREV;
            lv_obj_set_style_text_color(pic_last_label, lv_color_make(103,116,190), 0);
        }
    }
    else if (target == pic_next_label)              /* Previous image */
    {
        if (code == LV_EVENT_PRESSED)
        {
            lv_obj_set_style_text_color(pic_next_label, lv_color_hex(0x272727), 0);
        }
        else if (code == LV_EVENT_RELEASED)
        {
            pic_key = PIC_NEXT;
            lv_obj_set_style_text_color(pic_next_label, lv_color_make(103,116,190), 0);
        }
    }
}

/**
 * @brief  Photo album
 * @param  none
 * @return none
 */
void lv_pic_demo(void)
{
    pic_key = PIC_NULL;
    pic_frame = NULL;

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
        sd_check_en = 1;

        if (f_opendir(&picdir, "0:/PICTURE"))                       /* Open the music folder */
        {
            lv_msgbox("PICTURE folder error");
            return ;
        }
        
        pic_totpicnum = pic_get_tnum("0:/PICTURE");                 /* Get the total number of valid files */

        if (pic_totpicnum == 0)
        {
            lv_msgbox("No pic files");
            return ;
        }

        pic_picfileinfo = (FILINFO *)malloc(sizeof(FILINFO));       /* Allocate memory */
        pic_pname = malloc(255 * 2 + 1);                            /* Allocate memory for the file name with path */
        pic_picoffsettbl = malloc(4 * pic_totpicnum);               /* Allocate 4*totpicnum bytes to store the image index */

        if (!pic_picfileinfo || !pic_pname || !pic_picoffsettbl)    /* Memory allocation error */
        {
            lv_msgbox("memory allocation failed");
            return ;
        }

        lv_app_del();
        /* Create the main container of this screen */
        lv_obj_t *pic_obj = lv_obj_create(lv_scr_act());
        lv_obj_set_size(pic_obj,lv_obj_get_width(lv_scr_act()),lv_obj_get_height(lv_scr_act()));
        lv_obj_set_style_bg_color(pic_obj, lv_color_make(0,0,0), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(pic_obj,LV_OPA_100,LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(pic_obj,LV_OPA_0,LV_STATE_DEFAULT);
        lv_obj_set_pos(pic_obj,0,0);
        lv_obj_clear_flag(pic_obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_page_tile(pic_obj,"Image Browser");                       /* Set the page title */
        
        pic_name = lv_label_create(pic_obj);
        lv_obj_align(pic_name,LV_ALIGN_OUT_TOP_LEFT,0,42);
        lv_obj_set_width(pic_name,lv_obj_get_width(lv_scr_act()));
        lv_obj_set_style_text_font(pic_name,&myFont12,LV_STATE_DEFAULT);
        lv_label_set_long_mode(pic_name,LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_style_text_color(pic_name,lv_color_hex(0xff0000),LV_STATE_DEFAULT);
        lv_label_set_text(pic_name," ");

        pic_frame = lv_img_create(pic_obj);                         /* Create the image */
        lv_obj_set_style_bg_color(pic_frame, lv_color_make(50,52,67), LV_STATE_DEFAULT);
        lv_obj_align(pic_frame,LV_ALIGN_CENTER,0,20);               /* Center alignment */
        lv_obj_move_background(pic_frame);

        app_obj_general.current_parent = pic_obj;                   /* Point to the current screen container */
        app_obj_general.Function = lv_pic_del;                      /* Delete this screen */

        /* Previous song label */
        pic_last_label = lv_label_create(pic_obj);
        lv_obj_set_style_text_font(pic_last_label, &lv_font_montserrat_28, 0);  /* Set font */
        lv_label_set_text(pic_last_label, LV_SYMBOL_PREV);
        lv_obj_set_style_text_color(pic_last_label,lv_palette_main(LV_PALETTE_RED),LV_STATE_FOCUSED);
        lv_obj_align(pic_last_label,LV_ALIGN_LEFT_MID,10,0);
        lv_obj_set_style_text_color(pic_last_label, lv_color_make(103,116,190), 0);
        lv_obj_add_event_cb(pic_last_label, pic_play_event_cb, LV_EVENT_ALL, NULL);

        /* Next song label */
        pic_next_label = lv_label_create(pic_obj);
        lv_obj_set_style_text_font(pic_next_label, &lv_font_montserrat_28, 0);  /* Set font */
        lv_obj_set_style_text_color(pic_next_label,lv_palette_main(LV_PALETTE_RED),LV_STATE_FOCUSED);
        lv_label_set_text(pic_next_label, LV_SYMBOL_NEXT);
        lv_obj_align(pic_next_label,LV_ALIGN_RIGHT_MID,-10,0);
        lv_obj_set_style_text_color(pic_next_label, lv_color_make(103,116,190), 0);
        lv_obj_add_event_cb(pic_next_label, pic_play_event_cb, LV_EVENT_ALL, NULL);

        lv_group_add_obj(ctrl_g, pic_last_label);
        lv_group_add_obj(ctrl_g, pic_next_label);
        lv_group_focus_obj(pic_last_label);                                     /* Focus */

        if (PICTask_Handler == NULL)
        {
            /* Create the PIC task */
            xTaskCreatePinnedToCore((TaskFunction_t )pic,                 /* Task function */
                                    (const char*    )"pic",               /* Task name */
                                    (uint16_t       )PIC_STK_SIZE,        /* Task stack size */
                                    (void*          )NULL,                /* Parameter passed to the task function */
                                    (UBaseType_t    )PIC_PRIO,            /* Task priority */
                                    (TaskHandle_t*  )&PICTask_Handler,    /* Task handle */
                                    (BaseType_t     ) 1);                 /* Core on which the task runs */
        }
    }
}
/**
 ****************************************************************************************************
 * @file        lv_file.h
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-11-04
 * @brief       File management system
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

#ifndef __LV_FILE_DEMO_H
#define __LV_FILE_DEMO_H

#include "lvgl.h"
#include "ff.h"
#include "lcd.h"
#include "app_ui.h"


#define LIST_SIZE    100   /* Set the total number of folders and files; user-defined */
#define FILE_SEZE    1992  /* Set the read file content size; user-defined */

typedef struct
{
    lv_obj_t * list;          /* List widget */
    lv_obj_t *lv_page_obj;    /* Title object */
    lv_obj_t *lv_client_page;   /* Title object */
    lv_obj_t * list_btn[LIST_SIZE]; /* Number of list buttons */
    uint8_t list_flie_nuber;  /* Stores the number of folders and files */
    FRESULT fr;                 /* File read return value */
    FF_DIR lv_dir;              /* Directory to read */
    FILINFO SD_fno;             /* File information structure */
    char *pname;              /* File name with path */
    char *lv_pname;           /* Get the file name */
    char *lv_pname_shift;     /* Intermediate storage for the file name */
    const char* lv_pash;      /* Get the path */
    int lv_suffix_flag;       /* Suffix detection flag */
    int lv_prev_file_flag;    /* Previous file path flag */
    char *lv_prev_file[LIST_SIZE];  /* Store file paths */
    const void *image_scr;    /* Detect whether it is a file, image, or document */
    lv_obj_t * lv_back_obj;   /* Back/menu object */
    lv_obj_t * lv_prev_btn;   /* Back button */
    lv_obj_t * lv_back_btn;   /* Menu button */
    lv_obj_t *lv_page_cont;   /* Container for displaying text */
    char rbuf[FILE_SEZE];     /* Size of the text data to read */
    lv_obj_t *lv_return_page; /* Page to display */
    lv_obj_t *lv_image_read;  /* Read image object */
}lv_file_struct;

/* Program assertion: used for debugging; when term is 1 there is no error, when term is 0 this function reports an error */
#define FILE_ASSERT(term)                                                                                   \
do                                                                                                          \
{                                                                                                           \
    if (!(term))                                                                                            \
    {                                                                                                       \
        printf("Assert failed. Condition(%s). [%s][%d]\r\n", term, __FUNCTION__, __LINE__);                 \
        while(1)                                                                                            \
        {                                                                                                   \
            ;                                                                                               \
        }                                                                                                   \
    }                                                                                                       \
} while (0)

/* Function declarations */
void lv_file_demo(void);
void lv_page_tile(lv_obj_t *parent,char * tile_name);
#endif
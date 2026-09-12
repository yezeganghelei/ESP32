/**
 ****************************************************************************************************
 * @file        lv_file.c
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

#include "lv_file_demo.h"


LV_FONT_DECLARE(myFont12)           /* Declare the myFont24 font, which is already flashed to SPIFLASH */
LV_FONT_DECLARE(myFont24)
LV_FONT_DECLARE(myFont18)

extern lv_indev_t *indev_keypad;                            /* Button group */

lv_file_struct lv_flie;
/* File suffixes; unknown suffixes can be added to this array */
char *lv_suffix [] ={".txt", ".avi", ".png", "jpeg", ".jpg", ".bin", ".gif", ".bmp", ".FON", ".dat", ".sif", ".BIN", ".xbf", ".ttf", ".wav", ".WAV"};
#define LV_SUFFIX(x)    (int)(sizeof(x)/sizeof(x[0])) /* Calculate the size of the lv_suffix array */

uint16_t lv_scan_files (const char *path, lv_obj_t *parent);
void lv_del_list(lv_obj_t *parent);
void lv_create_list(lv_obj_t *parent);
void list_init(lv_obj_t *parent);
lv_obj_t *lv_create_page(lv_obj_t *parent);
char *lv_pash_joint(void);

/**
 * @brief  Tell the file position
 * @param  fd: file pointer
 * @return Return the position
 */
long lv_tell(lv_fs_file_t *fd)
{
    uint32_t pos = 0;
    lv_fs_tell(fd, &pos);
    printf("\nlv_tcur pos is: %ld\n", pos);
    return pos;
}

/**
 * @brief  Read file contents
 * @param  path: file path
 * @return LV_FS_RES_OK: read success
 */
lv_fs_res_t lv_file_read(const char *path)
{
    uint32_t rsize = 0;
    lv_fs_file_t fd;
    lv_fs_res_t res;

    res = lv_fs_open(&fd, path, LV_FS_MODE_RD);
    
    if (res != LV_FS_RES_OK)
    {
        printf("open %s ERROR\n",path);
        return LV_FS_RES_UNKNOWN;
    }

    lv_tell(&fd);
    lv_fs_seek(&fd,0,LV_FS_SEEK_SET);
    lv_tell(&fd);
    res = lv_fs_read(&fd, lv_flie.rbuf, FILE_SEZE, &rsize);

    if (res != LV_FS_RES_OK)
    {
        printf("read %s ERROR\n",path);
        return LV_FS_RES_UNKNOWN;
    }

    lv_tell(&fd);
    
    lv_fs_close(&fd);
    
    return LV_FS_RES_OK;
}

/**
  * @brief  Page back button callback
  * @param  obj  : object
  * @param  event: event
  * @retval none
  */
void lv_btn_close_event(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);

    if (code == LV_EVENT_RELEASED)
    {
        if (lv_flie.lv_image_read != NULL)     /* Check whether the image path to free is empty */
        {
            lv_obj_del(lv_flie.lv_image_read); /* Delete the image object */
            lv_flie.lv_image_read = NULL;      /* Set the image object to NULL */
        }
        
        lv_flie.lv_prev_file_flag -- ;         /* Decrement the file back flag */
        
        lv_obj_del(lv_flie.lv_page_cont);      /* Delete the page when back is pressed */
    }
}

/**
  * @brief  Create the page
  * @param  parent: parent object
  * @retval none
  */
lv_obj_t *lv_create_page(lv_obj_t *parent)
{
    lv_flie.lv_page_cont = lv_obj_create(parent);                           /* Create the container */
    lv_obj_set_size(lv_flie.lv_page_cont, lcd_self.width, lcd_self.height);
    lv_obj_set_style_radius(lv_flie.lv_page_cont, 0, LV_STATE_DEFAULT);       /* Set the corner radius to 0 */
    lv_obj_clear_flag(lv_flie.lv_page_cont, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_clear_flag(lv_flie.lv_page_cont, LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_align_to(lv_flie.lv_page_cont, parent, LV_ALIGN_CENTER, 0, 0);
  
    lv_obj_t *lv_page_obj = lv_obj_create(lv_flie.lv_page_cont);    /* Create the back-button area */
    lv_obj_set_style_bg_color(lv_page_obj, lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
    lv_obj_align(lv_page_obj, LV_ALIGN_BOTTOM_MID, 0, 10);
    lv_obj_set_size(lv_page_obj, lcd_self.width, myFont24.line_height);

    lv_obj_t *lv_page_back_btn = lv_label_create(lv_page_obj);      /* Create a label as the back object */
    lv_obj_set_style_text_font(lv_page_back_btn, &myFont24, LV_STATE_DEFAULT);
    lv_label_set_text(lv_page_back_btn, "Back");
    lv_obj_align_to(lv_page_back_btn, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(lv_page_back_btn, LV_OBJ_FLAG_CLICKABLE);       /* Make the label clickable */
    lv_obj_add_event_cb(lv_page_back_btn, lv_btn_close_event, LV_EVENT_ALL, NULL); /* Set the callback */

    return lv_flie.lv_page_cont;
}

/**
  * @brief  Display a .txt file
  * @param  parent: parent object
  * @retval none
  */
void lv_show_filetxt(lv_obj_t *parent)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_set_width(label, lv_obj_get_width(parent));
    lv_label_set_text(label, (char *)lv_flie.rbuf); /* Display the read data */
    memset(lv_flie.rbuf, 0, sizeof(lv_flie.rbuf));
}

/**
  * @brief  Display a .bin image
  * @param  parent: parent object
  * @param  path: path
  * @retval none
  */
void lv_show_imgbin(lv_obj_t *parent, const char *path)
{
    lv_flie.lv_image_read = lv_img_create(parent);                          /* Create the image widget */   
    lv_img_set_src(lv_flie.lv_image_read,path);                             /* Set the image for the widget */
    lv_obj_align_to(lv_flie.lv_image_read, parent, LV_ALIGN_CENTER, 0, 0);  /* Set the widget alignment and relative coordinates */
}

/**
  * @brief  Concatenate file paths
  * @param  none
  * @retval none
  */
char *lv_pash_joint(void)
{
    lv_flie.lv_prev_file[lv_flie.lv_prev_file_flag] = (char *)lv_flie.lv_pash;/* Save the previous path in this array */
    lv_flie.lv_prev_file_flag ++;                              /* Increment the previous-path count flag */
  
    strcpy((char *)lv_flie.pname, lv_flie.lv_pash);            /* Copy the path (directory) */ 
    strcat((char *)lv_flie.pname, "/");                        /* Copy the path (directory) */ 
    strcat((char *)lv_flie.pname, (char *)lv_flie.lv_pname);   /* Append the file name */
    return lv_flie.pname;
}

/**
  * @brief  List button callback
  * @param  event: event
  * @retval none
  */
static void lv_list_btn_event(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);
  
    if(code == LV_EVENT_CLICKED)
    {
        for (int i = 0; i <= lv_flie.list_flie_nuber ;i++)  /* Poll the list items */
        {
            if (obj == lv_flie.list_btn[i]) /* Check which list item button was pressed */
            {   
                lv_flie.lv_pname = malloc(255);         /* Allocate memory for the file name */
                lv_flie.pname = malloc(255);               /* Allocate memory for the file name with path */

                lv_flie.lv_pname = (char *)lv_list_get_btn_text(lv_flie.list, lv_flie.list_btn[i]);  /* Get the list item value */
                
                for (int suffix = 0; suffix < LV_SUFFIX(lv_suffix); suffix ++)      /* Poll the file suffixes */
                {
                    if (strstr(lv_flie.lv_pname, lv_suffix[suffix]) != NULL)        /* If it is not a folder */
                    {
                        lv_flie.lv_suffix_flag = 0;                                 /* Set the suffix flag to 0 */
                        break;
                    }
                }

                if (lv_flie.lv_suffix_flag == 1)                                  /* A non-zero flag means folder handling */
                {   
                    lv_flie.lv_pash = lv_pash_joint();                            /* Pass the file path to the lv_pash parameter */
                    lv_del_list(lv_flie.list);                                    /* Delete the list */
                    lv_scan_files(lv_flie.pname, lv_scr_act());                   /* Recreate the file list */
                }
                else
                {
                    lv_flie.lv_suffix_flag = 1; /* Restore folder click handling */
                }
            }
        }
    }
}

/**
  * @brief  Read file names
  * @param  char* path: file path to scan
  * @retval FR_OK: success, otherwise failure
  */
uint16_t lv_scan_files (const char *path, lv_obj_t *parent)
{
    lv_flie.fr = f_opendir(&lv_flie.lv_dir, path);         /* Open the file directory */
    memset(lv_flie.list_btn, 0, sizeof(lv_flie.list_btn));   /* Clear the list item array */

    lv_flie.list_flie_nuber = 0;                           /* Set the file index to 0 */
    lv_create_list(parent);                                /* Create the list */

    if (lv_flie.fr == FR_OK)
    {   /* If opened successfully, read file names into the buffer in a loop */
        while(1)
        {   /* Read file names in a loop; the number of iterations equals the number of files in the SD card root directory */
            lv_flie.fr = f_readdir(&lv_flie.lv_dir, &lv_flie.SD_fno);   /* Read the file name */

            if ((lv_flie.fr) || lv_flie.SD_fno.fname[0] == 0) break;    /* Break the loop on a read error or after all files have been read */
            lv_flie.list_flie_nuber++;              /* Increment the file count */

            if (lv_flie.SD_fno.fattrib & AM_DIR)    /* A folder name was read */
            {
                /* Copy the file name to the buffer and print it */
                lv_flie.list_btn[lv_flie.list_flie_nuber] = lv_list_add_btn(lv_flie.list, LV_SYMBOL_DIRECTORY, lv_flie.SD_fno.fname);      /* Add a list item (folder) */
            }
            else /* A file name was read */
            {
                if (  strstr(lv_flie.SD_fno.fname,".png")  != NULL  /* Check the file name */
                    ||strstr(lv_flie.SD_fno.fname,".jpeg") != NULL
                    ||strstr(lv_flie.SD_fno.fname,".jpg")  != NULL
                    ||strstr(lv_flie.SD_fno.fname,".bmp")  != NULL
                    ||strstr(lv_flie.SD_fno.fname,".gif")  != NULL
                    ||strstr(lv_flie.SD_fno.fname,".avi")  != NULL)
                {
                    lv_flie.image_scr = LV_SYMBOL_IMAGE;
                }
                else
                {
                    lv_flie.image_scr = "\xEF\x80\x96";
                }
                
                lv_flie.list_btn[lv_flie.list_flie_nuber] = lv_list_add_btn(lv_flie.list, lv_flie.image_scr, lv_flie.SD_fno.fname);         /* Add a list item (file) */ 
            }
            
            lv_obj_set_style_pad_left(lv_flie.list_btn[lv_flie.list_flie_nuber], 5, LV_STATE_DEFAULT);   /* Set the left padding of the list item */
            lv_obj_set_style_pad_right(lv_flie.list_btn[lv_flie.list_flie_nuber], 5, LV_STATE_DEFAULT);  /* Set the right padding of the list item */
            lv_obj_add_event_cb(lv_flie.list_btn[lv_flie.list_flie_nuber], lv_list_btn_event, LV_EVENT_ALL, NULL); /* Set the list callback */
        }
        
        f_closedir(&lv_flie.lv_dir); /* Close the file directory */
    }
    
    return lv_flie.fr;  /* Return */
}

/**
  * @brief  Delete the list
  * @param  parent: parent object
  * @retval none
  */
void lv_del_list(lv_obj_t *parent)
{
    lv_obj_del(parent);   /* Delete the object */
    lv_flie.list = NULL;  /* Set the list to NULL */
}

/**
  * @brief  Create the list
  * @param  parent: parent object
  * @retval none
  */
void lv_create_list(lv_obj_t *parent)
{
    lv_flie.list = lv_list_create(parent);  /* Create the list */
    app_obj_general.del_parent = lv_flie.list;
    lv_obj_set_size(lv_flie.list, lcd_self.width, lcd_self.height - myFont24.line_height * 2 - 28);     /* Set the list size */
    lv_obj_align_to(lv_flie.list, lv_flie.lv_page_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);                 /* Set the list alignment */
    lv_obj_set_style_text_font(lv_flie.list, &myFont12, LV_STATE_DEFAULT);                              /* Set font */
    lv_obj_set_style_radius(lv_flie.list, 0, LV_STATE_DEFAULT);                                         /* Set the corner radius to 0 */
}

/**
  * @brief  Create the page title
  * @param  parent: parent object
  * @retval none
  */
void lv_page_tile(lv_obj_t *parent,char * tile_name)
{
    lv_flie.lv_page_obj = lv_obj_create(parent);
    lv_obj_set_size(lv_flie.lv_page_obj, lcd_self.width + 5, myFont12.line_height + 5);
    lv_obj_set_style_bg_color(lv_flie.lv_page_obj,lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lv_flie.lv_page_obj, 0, LV_STATE_DEFAULT);          /* Set the corner radius to 0 */
    lv_obj_set_pos(lv_flie.lv_page_obj, -2,18);
    lv_obj_set_style_bg_opa(lv_flie.lv_page_obj,LV_OPA_90,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(lv_flie.lv_page_obj,LV_GRAD_DIR_VER,LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(lv_flie.lv_page_obj,LV_OPA_0,LV_STATE_DEFAULT);

    lv_obj_t *lv_page_label = lv_label_create(lv_flie.lv_page_obj);
    lv_label_set_text(lv_page_label, tile_name);
    lv_obj_set_style_text_color(lv_page_label, lv_palette_main(LV_PALETTE_RED), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_page_label, &myFont12,LV_STATE_DEFAULT);
    lv_obj_align_to(lv_page_label, lv_flie.lv_page_obj, LV_ALIGN_CENTER, 0, -2);
}

/**
  * @brief  Back button callback
  * @param  obj: object
  * @param  event: event
  * @retval none
  */
void lv_back_btn_event_handler(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_target(event);
  
    if(code == LV_EVENT_SHORT_CLICKED)
    {
        if (obj == lv_flie.lv_back_btn)
        {
            lv_del_list(lv_flie.list);      /* Delete the list */
            list_init(lv_scr_act());        /* Return to the menu */
        }
        if (obj == lv_flie.lv_prev_btn)
        { 
            lv_flie.lv_prev_file_flag--;    /* Decrement the file back flag */

            if (lv_flie.lv_prev_file_flag <= 0)
            {
                lv_flie.lv_prev_file_flag = 0; /* If less than or equal to 0, force it to 0 */
            }
          
            lv_del_list(lv_flie.list);         /* Delete the list widget immediately */
            lv_flie.lv_pash = lv_flie.lv_prev_file[lv_flie.lv_prev_file_flag];              /* Copy the parent path to the lv_pash parameter */
            lv_scan_files(lv_flie.lv_prev_file[lv_flie.lv_prev_file_flag], lv_scr_act());   /* Read the file path */
        }
    }
}

/**
  * @brief  Back button
  * @param  parent: parent object
  * @retval none
  */
void lv_general_win_create(lv_obj_t *parent)
{
    lv_flie.lv_back_btn = lv_label_create(parent);
    lv_obj_set_style_text_font(lv_flie.lv_back_btn, &myFont24, LV_STATE_DEFAULT);   /* Set font */

    lv_label_set_text(lv_flie.lv_back_btn, "Menu");
    lv_obj_set_style_text_color(lv_flie.lv_back_btn,lv_color_white(),LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lv_flie.lv_back_btn,lv_palette_main(LV_PALETTE_RED),LV_STATE_FOCUSED);
    lv_obj_align_to(lv_flie.lv_back_btn, parent, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_add_flag(lv_flie.lv_back_btn, LV_OBJ_FLAG_CLICKABLE);                    /* Make the label clickable */
    lv_obj_add_event_cb(lv_flie.lv_back_btn, lv_back_btn_event_handler, LV_EVENT_ALL, NULL); /* Set the callback */

    
    lv_flie.lv_prev_btn = lv_label_create(parent);
    lv_obj_set_style_text_font(lv_flie.lv_prev_btn, &myFont24, LV_STATE_DEFAULT);
    lv_label_set_text(lv_flie.lv_prev_btn, "Back");
    lv_obj_set_style_text_color(lv_flie.lv_prev_btn,lv_color_white(),LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lv_flie.lv_prev_btn,lv_palette_main(LV_PALETTE_RED),LV_STATE_FOCUSED);
    lv_obj_align_to(lv_flie.lv_prev_btn, parent, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_add_flag(lv_flie.lv_prev_btn, LV_OBJ_FLAG_CLICKABLE);        /* Make the label clickable */
    lv_obj_add_event_cb(lv_flie.lv_prev_btn, lv_back_btn_event_handler, LV_EVENT_ALL, NULL);

    lv_group_add_obj(ctrl_g, lv_flie.lv_prev_btn);
    lv_group_add_obj(ctrl_g, lv_flie.lv_back_btn);
    lv_group_focus_obj(lv_flie.lv_prev_btn);                            /* Focus */
}

/**
  * @brief  File system back/menu button area
  * @param  parent: parent object
  * @retval none
  */
void lv_page_back(lv_obj_t *parent)
{
    lv_flie.lv_back_obj = lv_obj_create(parent);                                                            /* Create the file back object area */
    lv_obj_set_size(lv_flie.lv_back_obj, lcd_self.width, myFont24.line_height + 10);                        /* Set the size of this area */
    lv_obj_set_style_bg_color(lv_flie.lv_back_obj,lv_palette_main(LV_PALETTE_BLUE), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lv_flie.lv_back_obj, 0, LV_STATE_DEFAULT);                                      /* Set the corner radius to 0 */
    lv_obj_align_to(lv_flie.lv_back_obj, parent, LV_ALIGN_BOTTOM_MID, 0, 5);                                /* Set the alignment */
    lv_obj_set_style_bg_opa(lv_flie.lv_back_obj,LV_OPA_90,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(lv_flie.lv_back_obj,LV_GRAD_DIR_VER,LV_STATE_DEFAULT);
    lv_general_win_create(lv_flie.lv_back_obj);                                                             /* Create the back and menu buttons */
    lv_obj_set_style_border_opa(lv_flie.lv_back_obj,LV_OPA_0,LV_STATE_DEFAULT);
}

/**
  * @brief  List initialization
  * @param  parent: parent object
  * @retval none
  */
void list_init(lv_obj_t *parent)
{
    lv_flie.lv_pash = "0:";                                 /* Initial path */
    lv_flie.lv_prev_file_flag = 0;                          /* Clear the previous file path flag */
    lv_flie.lv_prev_file[lv_flie.lv_prev_file_flag] = "0:"; /* Initial previous folder path */
    lv_flie.list_flie_nuber = 0;                            /* Initial file count */
    lv_flie.lv_suffix_flag = 1;                             /* File suffix flag */
    lv_scan_files(lv_flie.lv_pash,parent);                  /* Read the file path */
}

/**
  * @brief  Delete the file demo
  * @param  none
  * @retval none
  */
void lv_file_del(void)
{
    if (app_obj_general.del_parent != NULL)
    {
        lv_del_list(lv_flie.list);
    }

    app_obj_general.del_parent = NULL;

    lv_group_remove_obj(lv_flie.lv_prev_btn);
    lv_group_remove_obj(lv_flie.lv_back_btn);
    lv_obj_clean(app_obj_general.current_parent);
    lv_obj_del(app_obj_general.current_parent);
    app_obj_general.current_parent = NULL;
    lv_app_show();
}

/**
  * @brief  Widget test function
  * @param  none
  * @retval none
  */
void lv_file_demo(void)
{
    esp_err_t err = ESP_OK;

    /* Initialize the SD card */
    err = sd_spi_init();
    vTaskDelay(200);

    /* Handle repeated presses */
    if (app_obj_general.current_parent != NULL)
    {
        lv_obj_del(app_obj_general.current_parent);
        app_obj_general.current_parent = NULL;
    }

    if (err != ESP_OK)
    {
        lv_msgbox("SD device not detected");
    }
    else
    {
        lv_app_del();
        /* Create the main container of this screen */
        lv_obj_t *file_obj = lv_obj_create(lv_scr_act());
        lv_obj_set_style_radius(file_obj, 0, LV_STATE_DEFAULT);
        lv_obj_set_size(file_obj,lv_obj_get_width(lv_scr_act()),lv_obj_get_height(lv_scr_act()));
        lv_obj_set_style_border_opa(file_obj,LV_OPA_0,LV_STATE_DEFAULT);
        lv_obj_set_pos(file_obj,0,0);
        lv_obj_clear_flag(file_obj, LV_OBJ_FLAG_SCROLLABLE);

        app_obj_general.current_parent = file_obj;              /* Point to the current screen container */
        app_obj_general.Function = lv_file_del;                 /* Delete this screen */
        lv_page_tile(file_obj,"TF Card Browser");                           /* Set the page title */
        lv_page_back(file_obj);                                 /* Set the page back */
        list_init(file_obj);                                    /* List initialization */
    }
}

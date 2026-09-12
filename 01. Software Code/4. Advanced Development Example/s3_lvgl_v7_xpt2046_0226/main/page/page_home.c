/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-05 09:04:44
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2021-11-13 17:08:36
 * @FilePath     : \s3_lvgl_v7\main\page\page_home.c
 */
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "lv_port_indev.h"
#include "esp_err.h"
#include "gui_time.h"

#include "bili_http.h"
#include "page_home.h"
#include "page_menu.h"
#include "page_start.h"
#include "myFont.h"
#include "number_temp.h"
#include "number.h"
#include "myimg.h"
#include "weather.h"
#include "button.h"
#include "lv_gif.h"

LV_IMG_DECLARE(taikong001);
LV_IMG_DECLARE(taikong002);
LV_IMG_DECLARE(taikong003);
LV_IMG_DECLARE(taikong004);
LV_IMG_DECLARE(taikong005);
LV_IMG_DECLARE(taikong006);
LV_IMG_DECLARE(taikong007);
LV_IMG_DECLARE(taikong008);
LV_IMG_DECLARE(taikong009);
LV_IMG_DECLARE(taikong010);

extern struct DATA_WEATHER weather_data[3];
extern const struct LV_WEATHER lv_weather[40];
extern struct _ksdiy_sys_t ksdiy_sys_t;

LV_IMG_DECLARE(bili_half);
LV_IMG_DECLARE(bili_half);
LV_IMG_DECLARE(kevincoooool);
LV_FONT_DECLARE(myFont);
LV_FONT_DECLARE(number);
LV_FONT_DECLARE(number_temp);

/*This page's window*/
static lv_obj_t *appWindow;

extern lv_obj_t *scr;
extern lv_obj_t *scr_body;
extern lv_obj_t *cont_head;

static lv_obj_t *img_weather;

lv_style_t style1;
/*****Main body****/
lv_obj_t *cont_time, *cont_temp, *cont_fensi, *img_anim;
/*****Time****/
lv_obj_t *label_time1, *label_time1_shi, *label_time1_fen;
lv_obj_t *label_time2;
lv_task_t *task_time, *task_anim;
/*****Followers****/
lv_obj_t *label_fensi;
lv_obj_t *label_guanzhu;
/*****Temperature and humidity****/
lv_obj_t *cont_temp_number;
lv_obj_t *label_temp_number;

static void update_weather(void);
void page_task_time(lv_task_t *task)
{
    static uint8_t i = 0, h = 0;
    if (i % 2 == 0)
        lv_obj_set_hidden(label_time1, 1);
    else
        lv_obj_set_hidden(label_time1, 0);

    if (i % 20 == 0) //10s
    {
        update_temp(); //Update temperature and humidity
        update_time(); //Update time
    }
    // if (i % 500 == 0) // ten minutes
    //     read_fans();
    // if (i % (1000) == 0) // 12 hours
    // {
    //     read_weather();
    // }
    if (i % 120 == 0) //One minute
    {
        i = 0;
        h++;
        if (h % 3 == 0) //3 minutes, update the follower count
            update_fensi();
        if (h == h * 60 * 8) //8 hours
        {
            update_weather(); //Update weather
            h = 0;
        }
    }
    i++;
}
void update_time()
{
    char now_time[40];
    sprintf((char *)now_time, "#FFFFFF %02d#", ksdiy_sys_t.timeinfo.tm_hour);
    lv_label_set_text(label_time1_shi, (const char *)now_time);
    sprintf((char *)now_time, "#FFFFFF %02d#", ksdiy_sys_t.timeinfo.tm_min);
    lv_label_set_text(label_time1_fen, (const char *)now_time);
    //Weekday and date
    switch (ksdiy_sys_t.timeinfo.tm_wday)
    {
    case 0:
        sprintf((char *)now_time, "Sunday\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
    case 1:
        sprintf((char *)now_time, "Monday\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
    case 2:
        sprintf((char *)now_time, "Tuesday\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
    case 3:
        sprintf((char *)now_time, "Wednesday\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
    case 4:
        sprintf((char *)now_time, "Thursday\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
    case 5:
        sprintf((char *)now_time, "Friday\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
    case 6:
        sprintf((char *)now_time, "Saturday\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;

    default:
        sprintf((char *)now_time, "Sunday\n %02d/%02d", ksdiy_sys_t.timeinfo.tm_mon + 1, ksdiy_sys_t.timeinfo.tm_mday);
        break;
        break;
    }
    lv_label_set_text(label_time2, (const char *)now_time);
}
void page_home_time(void)
{
    cont_time = lv_cont_create(scr_body, NULL);
    lv_cont_set_fit2(cont_time, LV_FIT_NONE, LV_FIT_NONE);
    lv_obj_set_size(cont_time, 130, 130);
    lv_obj_set_pos(cont_time, 0, 10);
    lv_obj_add_style(cont_time, LV_BTN_PART_MAIN, &style1); /*Default button style*/
    //lv_cont_set_layout(cont_time, LV_LAYOUT_CENTER);
    //Hour
    label_time1_shi = lv_label_create(cont_time, NULL);
    lv_label_set_recolor(label_time1_shi, true);
    lv_obj_set_pos(label_time1_shi, 2, 0);
    lv_label_set_align(label_time1_shi, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_obj_set_style_local_text_font(label_time1_shi, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &number);

    //Colon
    label_time1 = lv_label_create(cont_time, NULL);
    lv_label_set_recolor(label_time1, true);
    lv_obj_set_pos(label_time1, 58, -5);
    lv_label_set_align(label_time1, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_obj_set_style_local_text_font(label_time1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &number);
    lv_label_set_text(label_time1, "#FFA500 :#");
    //Minute
    label_time1_fen = lv_label_create(cont_time, NULL);
    lv_label_set_recolor(label_time1_fen, true);
    lv_obj_set_pos(label_time1_fen, 68, 0);
    lv_label_set_align(label_time1_fen, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_obj_set_style_local_text_font(label_time1_fen, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &number);

    //Weekday and date
    label_time2 = lv_label_create(cont_time, NULL);
    lv_label_set_recolor(label_time2, true);
    lv_obj_set_pos(label_time2, 0, 70);
    lv_label_set_align(label_time2, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_obj_set_style_local_text_font(label_time2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
    lv_obj_set_style_local_text_color(label_time2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    update_time(); //Update data

    //Animation

    obj_add_anim(
        cont_time,                        //Animation object
        (lv_anim_exec_xcb_t)lv_obj_set_x, //Animation function
        300,                              //Animation speed
        -(APP_WIN_WIDTH / 2),              //Start value
        10,                               //End value
        lv_anim_path_ease_out             //Animation effect: simulate a bouncing object falling
    );
}
lv_img_dsc_t bili_face = {
    .header.always_zero = 0,
    .header.w = 300,
    .header.h = 300,
    .data_size = 300 * 300 * 2,
    .header.cf = LV_IMG_CF_RAW,
    .data = NULL,
};
lv_obj_t *img_anim, *img_face, *face_cont;
LV_IMG_DECLARE(png_decoder_test);

extern const uint8_t taikongren_map[];
void page_home_anim(void)
{
    img_anim = lv_img_create(scr_body, NULL);
    //Write style LV_IMG_PART_MAIN for WIN_img0
    static lv_style_t style_img;
    lv_style_init(&style_img);

    //Write style state: LV_STATE_DEFAULT for style_img
    lv_style_set_image_recolor(&style_img, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_image_recolor_opa(&style_img, LV_STATE_DEFAULT, 0);
    lv_style_set_image_opa(&style_img, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(img_anim, LV_IMG_PART_MAIN, &style_img);
    lv_obj_set_pos(img_anim, APP_WIN_WIDTH / 2 + 60, 70);
    lv_img_set_src(img_anim, &taikong001);

        img_face = lv_img_create(scr_body, NULL);
    lv_obj_add_style(img_face, LV_IMG_PART_MAIN, &style_img);
    lv_obj_set_pos(img_face, APP_WIN_WIDTH / 2 - 40, 70);


    bili_face.data = (uint8_t *)face_buffer;//Assign the buffer fetched online to the image

    lv_img_set_src(img_face, &bili_face);    // lv_obj_set_size(img_anim, 100, 100);
    // lv_obj_set_pos(img_anim, 70, 50);
    //Animation

    obj_add_anim(
        img_anim,                         //Animation object
        (lv_anim_exec_xcb_t)lv_obj_set_y, //Animation function
        300,                              //Animation speed
        APP_WIN_HEIGHT,                     //Start value
        APP_WIN_HEIGHT - 170,               //End value
        lv_anim_path_ease_out             //Animation effect: simulate a bouncing object falling
    );
}
void update_fensi()
{
    char now_time[40];
    sprintf((char *)now_time, "Fans:%d\nFollowing:%d", ksdiy_sys_t.wp.follow, ksdiy_sys_t.wp.following);
    lv_label_set_text(label_fensi, (const char *)now_time);
}
void page_home_fensi(void)
{

    cont_fensi = lv_cont_create(scr_body, NULL);
    lv_cont_set_fit2(cont_fensi, LV_FIT_NONE, LV_FIT_NONE);
    lv_obj_set_size(cont_fensi, 100, 48);
    lv_obj_set_pos(cont_fensi, APP_WIN_WIDTH / 2 - 50, APP_WIN_HEIGHT - 80);
    lv_obj_add_style(cont_fensi, LV_BTN_PART_MAIN, &style1); /*Default button style*/
    lv_cont_set_layout(cont_fensi, LV_LAYOUT_CENTER);
    //Theme
    static lv_style_t label_fensi_style;
    lv_style_init(&label_fensi_style);
    lv_style_set_bg_opa(&label_fensi_style, LV_STATE_DEFAULT, 0);
    lv_style_set_text_opa(&label_fensi_style, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_text_color(&label_fensi_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    //Followers
    label_fensi = lv_label_create(cont_fensi, NULL);
    lv_label_set_recolor(label_fensi, true);
    lv_label_set_align(label_fensi, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_obj_set_style_local_text_font(label_fensi, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
    lv_obj_add_style(label_fensi, LV_LABEL_PART_MAIN, &label_fensi_style);
    update_fensi();

    //Animation
    obj_add_anim(
        cont_fensi,                       //Animation object
        (lv_anim_exec_xcb_t)lv_obj_set_y, //Animation function
        300,                              //Animation speed
        APP_WIN_HEIGHT,                     //Start value
        APP_WIN_HEIGHT - 80,                //End value
        lv_anim_path_ease_out             //Animation effect: simulate a bouncing object falling
    );
}
void update_temp()
{
    //Update temperature and humidity
    char now_time[40];
    sprintf(now_time, "△%d℃\n▽%d℃", weather_data[0].high, weather_data[0].low);
    // printf("now temp:\n%s\n", now_time);
    lv_label_set_text(label_temp_number, (const char *)now_time);
}
static void update_weather()
{
    //Weather image

    ESP_LOGI("WEATHER", "Updating weather");

    lv_img_set_src(img_weather, lv_weather[weather_data[0].code_day].img_dsc);
}
/*Temperature*/
void page_home_temp(void)
{

    //Main container
    cont_temp = lv_cont_create(scr_body, NULL);
    lv_cont_set_fit2(cont_temp, LV_FIT_NONE, LV_FIT_NONE);
    lv_obj_set_size(cont_temp, 110, 50);
    lv_obj_set_pos(cont_temp, APP_WIN_WIDTH - 110, 15);

    lv_obj_add_style(cont_temp, LV_BTN_PART_MAIN, &style1); /*Default button style*/

    img_weather = lv_img_create(cont_temp, NULL);
    uint8_t i = weather_data[0].code_day;
    lv_img_dsc_t *p = (lv_img_dsc_t *)lv_weather[i].img_dsc;
    lv_img_set_src(img_weather, p);
    lv_obj_set_pos(img_weather, 60, 0);
    //Style
    static lv_style_t label_temp_style;
    lv_style_init(&label_temp_style);
    lv_style_set_text_opa(&label_temp_style, LV_STATE_DEFAULT, 255);
    lv_style_set_text_color(&label_temp_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    //Percentage
    label_temp_number = lv_label_create(cont_temp, NULL);
    // lv_obj_set_size(label_temp_number, 70, 50);
    // lv_obj_set_pos(label_temp_number, 130, 15);
    lv_obj_set_size(label_temp_number, 60, 50);
    lv_obj_set_pos(label_temp_number, 8, 0);
    lv_label_set_align(label_temp_number, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_obj_set_style_local_text_font(label_temp_number, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
    lv_obj_add_style(label_temp_number, LV_LABEL_PART_MAIN, &label_temp_style);

    update_temp();
    update_weather();
    //Load animation
    obj_add_anim(
        cont_temp,                        //Animation object
        (lv_anim_exec_xcb_t)lv_obj_set_x, //Animation function
        500,                              //Animation speed
        APP_WIN_WIDTH,                     //Start value
        APP_WIN_WIDTH - 110,               //End value
        lv_anim_path_ease_out             //Animation effect: simulate a bouncing object falling
    );
}

void page_home_ksdiy()
{

    static lv_obj_t *img22;
    img22 = lv_img_create(scr_body, NULL);
    lv_img_set_src(img22, &bili_half);
    lv_obj_set_pos(img22, -(APP_WIN_WIDTH / 2), APP_WIN_HEIGHT - 80);

    static lv_obj_t *img33;
    img33 = lv_img_create(scr_body, NULL);
    lv_img_set_src(img33, &bili_half);
    lv_obj_set_pos(img33, APP_WIN_WIDTH, APP_WIN_HEIGHT - 80);

    obj_add_anim(
        img22,                            //Animation object
        (lv_anim_exec_xcb_t)lv_obj_set_x, //Animation function
        500,                              //Animation speed
        -(APP_WIN_WIDTH / 2),              //Start value
        10,                               //End value
        lv_anim_path_ease_out             //Animation effect: simulate a bouncing object falling
    );
    obj_add_anim(
        img33,                            //Animation object
        (lv_anim_exec_xcb_t)lv_obj_set_x, //Animation function
        500,                              //Animation speed
        APP_WIN_WIDTH,                     //Start value
        APP_WIN_WIDTH - 60,                //End value
        lv_anim_path_ease_out             //Animation effect: simulate a bouncing object falling
    );
}

static void event_handler_page_home(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_GESTURE)
    {

        switch (lv_indev_get_gesture_dir(lv_indev_get_act()))
        {
        case LV_GESTURE_DIR_TOP:

            printf("LV_GESTURE_DIR_TOP.\n\r");
            page.PagePush(Disp_Menu);
            break;
        case LV_GESTURE_DIR_BOTTOM:

            printf("LV_GESTURE_DIR_BOTTOM.\n\r");
            break;
        case LV_GESTURE_DIR_RIGHT:

            printf("LV_GESTURE_DIR_RIGHT.\n\r");
            break;
        case LV_GESTURE_DIR_LEFT:
      
            printf("LV_GESTURE_DIR_LEFT.\n\r");
            break;
        default:
            break;
        }
    }
    switch (event)
    {
    case LV_EVENT_PRESSED: /* Pressed */
        printf("Pressed\n");
        break;

    case LV_EVENT_SHORT_CLICKED: /* Short click */
        printf("Short clicked\n");
        break;

    case LV_EVENT_CLICKED: /* Click */

        printf("Clicked\n");
        break;

    case LV_EVENT_LONG_PRESSED: /* Long press */
    
        printf("Long press\n");
        break;

    case LV_EVENT_LONG_PRESSED_REPEAT: /* Long press repeat */
        printf("Long press repeat\n");
        break;

    case LV_EVENT_RELEASED: /* Released */
        printf("Released\n");
        break;
    }
}

void page_home_load()
{

    lv_style_set_bg_color(&style1, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_color(&style1, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_width(&style1, LV_STATE_DEFAULT, 0);
    lv_style_set_border_opa(&style1, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_opa(&style1, LV_STATE_DEFAULT, 0);
    page_home_ksdiy();
    page_home_temp();
    page_home_fensi();
    page_home_time();
    page_home_anim();
    ANIEND
    /* Assign the screen touch event handler */
    lv_obj_set_click(lv_layer_top(), true);
    lv_obj_set_event_cb(lv_layer_top(), event_handler_page_home);
}
static void Exit(void)
{
    lv_task_del(task_time);
    lv_task_del(task_anim);
    lv_task_handler();

    ANIEND
    lv_obj_clean(scr_body); //Clear the page scr_body
    lv_obj_set_click(lv_layer_top(), false);
    lv_obj_clean(lv_layer_top());
    lv_obj_set_event_cb(lv_layer_top(), NULL); /* Assign the event handler */

    ESP_LOGI("SYSTEM", "esp_get_free_heap_size : %d  ", esp_get_free_heap_size());
}

void page_anim_time(lv_task_t *task)
{
    static uint8_t i = 1;
    switch (i)
    {
    case 1:
        lv_img_set_src(img_anim, &taikong001);
        break;
    case 2:
        lv_img_set_src(img_anim, &taikong002);
        break;
    case 3:
        lv_img_set_src(img_anim, &taikong003);
        break;
    case 4:
        lv_img_set_src(img_anim, &taikong004);
        break;
    case 5:
        lv_img_set_src(img_anim, &taikong005);
        break;
    case 6:
        lv_img_set_src(img_anim, &taikong006);
        break;
    case 7:
        lv_img_set_src(img_anim, &taikong007);
        break;
    case 8:
        lv_img_set_src(img_anim, &taikong008);
        break;
    case 9:
        lv_img_set_src(img_anim, &taikong009);
        break;
    case 10:
        lv_img_set_src(img_anim, &taikong010);
        break;
    default:
        break;
    }

    if (i == 10)
        i = 1;
    i++;
}
static void Setup(void)
{
    page_home_load();
    task_time = lv_task_create(page_task_time, 500, LV_TASK_PRIO_MID, NULL); //Create task
    task_anim = lv_task_create(page_anim_time, 100, LV_TASK_PRIO_MID, NULL); //Create task

    ESP_LOGI("SYSTEM", "esp_get_free_heap_size : %d ", esp_get_free_heap_size());
}
extern struct _ksdiy_sys_t ksdiy_sys_t;
void move_task_home(uint8_t move)
{
    switch (move)
    {

    case BT1_DOWN: //Move up

        break;
    case BT1_DOUBLE: //Move up

        break;
    case BT1_LONG: //Move up

        break;
    case BT1_LONGFREE: //Move up

        break;
    case BT2_DOWN:

        break;
    case BT3_LONG: //Move down

        break;
    case BT3_LONGFREE: //Move up

        break;

    case BT3_DOWN: //Move down

        break;

    default:
        break;
    }
}

/**
  * @brief  Page event
  * @param  btn:button that raised the event
  * @param  event:event ID
  * @retval None
  */
static void Event(void *btn, int event)
{
    // if(btn == &btOK)
    // {
    //     if(event == ButtonEvent::EVENT_ButtonLongPressed)
    //     {
    //         /* Long press OK to exit the previous page */
    //         page.PagePop();
    //     }
    //     else if(event == ButtonEvent::EVENT_ButtonClick)
    //     {
    //         /* Single click OK to enter the corresponding page */
    //         uint8_t pageID = ICON_Grp[ICON_NowSelIndex].pageID;
    //         if(pageID != PAGE_NONE)
    //         {
    //             page.PagePush(pageID);
    //         }
    //     }
    // }

    // if(event == ButtonEvent::EVENT_ButtonPress || event == ButtonEvent::EVENT_ButtonLongPressRepeat)
    // {
    //     /* Press or long-press the up/down keys to select icons up/down */
    //     if(btn == &btUP)
    //     {
    //         ICON_Grp_Move(-1);
    //     }
    //     if(btn == &btDOWN)
    //     {
    //         ICON_Grp_Move(+1);
    //     }
    // }
}

/**
  * @brief  Page registration
  * @param  pageID:ID assigned to this page
  * @retval None
  */
void PageRegister_Home(uint8_t pageID)
{
    /*Get the window assigned to this page*/
    // appWindow = AppWindow_GetCont(pageID);

    /*Register with the page scheduler*/
    page.PageRegister(pageID, Setup, NULL, Exit, NULL);
    printf("/* Register Home with the page scheduler */\r\n");
}

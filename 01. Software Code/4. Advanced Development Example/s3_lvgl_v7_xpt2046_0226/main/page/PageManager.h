/*
 * @Author: your name
 * @Date: 2021-11-06 18:32:15
 * @LastEditTime: 2021-11-13 14:45:09
 * @LastEditors: Please set LastEditors
 * @Description: Open koroFileHeader to view the configuration and settings: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \s3_lvgl_v7\main\page\spage.h
 */
/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-10 13:58:24
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-08-17 14:54:57
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\spage.h
 */
#ifndef _SPAGE_
#define _SPAGE_
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "app_anim.h"
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "../../lvgl/lvgl.h"
#include "../../lv_ex_conf.h"
#endif


void lv_ksdiy_web_init(void);
void lv_ksdiy_cam_init(void);
void clock_switch_save(uint8_t i);
void web_switch_save(uint8_t i);
void cam_switch_save(uint8_t i);
void cam_vflip_switch_save(uint8_t i);
void cam_hmirror_switch_save(uint8_t i);
void web_temp_save(uint8_t i);
void ksdiy_sys_info_init(void);

void lv_ico_web_show(uint8_t state);
void lv_ico_clock_show(uint8_t state);
void lv_ico_temp_show(uint8_t state);
void save_clock(uint8_t clock_index, uint8_t s, uint8_t h, uint8_t m);




typedef void (*CallbackFunction_t)(void);
typedef void (*EventFunction_t)(void *, int);
typedef struct
{
    CallbackFunction_t SetupCallback;
    CallbackFunction_t LoopCallback;
    CallbackFunction_t ExitCallback;
    EventFunction_t EventCallback;
} PageList_TypeDef;

typedef struct
{
    void (*PageManager)(uint8_t pageMax, uint8_t pageStackSize);

    uint8_t NowPage;
    uint8_t LastPage;
    uint8_t NextPage;
    uint8_t NewPage;
    uint8_t OldPage;

    bool (*PageRegister)(
        uint8_t pageID,
        CallbackFunction_t setupCallback,
        CallbackFunction_t loopCallback,
        CallbackFunction_t exitCallback,
        EventFunction_t eventCallback);
    bool (*PageClear)(uint8_t pageID);
    bool (*PagePush)(uint8_t pageID);
    bool (*PagePop)(void);
    void (*PageChangeTo)(uint8_t pageID);
    void (*PageEventTransmit)(void *obj, int event);
    void (*PageStackClear)(void);
    void (*Running)(void);

} _PageManager;
extern _PageManager page;

struct settingPage
{
    void (*onCreat)(void);  // Create and display the screen
    void (*onRelece)(void); // Destroy the screen and free the memory used by all its elements
    void (*hidePage)(int hide);
    void (*onKey)(int keyCode, int keyOption); // The first parameter is the button type (which button was pressed); the second is the button action (pressed or released)
};
void Page_StackInit(void);
void Page_StackPush(struct settingPage element);
void Page_StackPop(void);
struct settingPage Page_StackGetTop();
void Page_StackDestroy();
bool Is_Empty(void);
void SettingCreat(void);
void Back_To_Last(void);

#endif // _TEST_

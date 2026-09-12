#ifndef WEATHER_H_
#define WEATHER_H_
#define JINTIAN "Today"
#define MINGTIAN "Tomorrow"
#define HOUTIAN "Day After Tomorrow"
#include "lvgl/lvgl.h"
struct WEATHER{
    char * weathername;
    char * filename;
};
struct LV_WEATHER{
    char * weathername;
    const lv_img_dsc_t * img_dsc;
};
#endif
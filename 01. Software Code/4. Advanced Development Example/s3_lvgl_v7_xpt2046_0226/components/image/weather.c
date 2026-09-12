#include "weather.h"
LV_IMG_DECLARE(weather0);
LV_IMG_DECLARE(weather1);
LV_IMG_DECLARE(weather2);
LV_IMG_DECLARE(weather4);
LV_IMG_DECLARE(weather5);
LV_IMG_DECLARE(weather6);
LV_IMG_DECLARE(weather10);
LV_IMG_DECLARE(weather11);
LV_IMG_DECLARE(weather13);
LV_IMG_DECLARE(weather14);
LV_IMG_DECLARE(weather15);
LV_IMG_DECLARE(weather16);
LV_IMG_DECLARE(weather19);
LV_IMG_DECLARE(weather20);
LV_IMG_DECLARE(weather21);
LV_IMG_DECLARE(weather22);
LV_IMG_DECLARE(weather24);
LV_IMG_DECLARE(weather26);
LV_IMG_DECLARE(weather23);
LV_IMG_DECLARE(weather28);
LV_IMG_DECLARE(weather30);
LV_IMG_DECLARE(weather31);
LV_IMG_DECLARE(weather32);
LV_IMG_DECLARE(weather35);
LV_IMG_DECLARE(weather36);
LV_IMG_DECLARE(weather40);
// const struct WEATHER weather[40]={//https://docs.seniverse.com/api/start/code.html
//     {"Sunny","0.png"},//0
//     {"Sunny","1.png"},
//     {"Sunny","0.png"},
//     {"Sunny","1.png"},//3
//     {"Cloudy","4.png"},//4
//     {"Partly Cloudy","5.png"},//5
//     {"Partly Cloudy","6.png"},//6
//     {"Mostly Cloudy","5.png"},//7
//     {"Mostly Cloudy","6.png"},//8
//     {"Overcast","4.png"},//9
//     {"Shower","10.png"},//10
//     {"Thunder Shower","11.png"},//11
//     {"Thunderstorm with Hail","12.png"},//12
//     {"Light Rain","13.png"},//13
//     {"Moderate Rain","14.png"},//14
//     {"Heavy Rain","16.png"},//15
//     {"Rainstorm","16.png"},//16
//     {"Heavy Rainstorm","15.png"},//17
//     {"Severe Rainstorm","18.png"},//18
//     {"Freezing Rain","19.png"},//19
//     {"Sleet","20.png"},//20
//     {"Snow Shower","21.png"},//21
//     {"Light Snow","22.png"},//22
//     {"Moderate Snow","23.png"},//23
//     {"Heavy Snow","24.png"},//24
//     {"Snowstorm","24.png"},//25
//     {"Dust","26.png"},//26
//     {"Sand","26.png"},//27
//     {"Sandstorm","28.png"},//28
//     {"Severe Sandstorm","28.png"},//29
//     {"Fog","30.png"},//30
//     {"Haze","31.png"},//31
//     {"Windy","32.png"},//32
//     {"Strong Wind","32.png"},//33
//     {"Hurricane","32.png"},//34
//     {"Tropical Storm","35.png"},//35
//     {"Tornado","36.png"},//36
//     {"Cold","40.png"},//37
//     {"Hot","40.png"},//38
//     {"Unknown","40.png"},//39
// };
const struct LV_WEATHER lv_weather[40]={//https://docs.seniverse.com/api/start/code.html
    {"Sunny",&weather0},//0
    {"Sunny",&weather1},
    {"Sunny",&weather0},
    {"Sunny",&weather1},//3
    {"Cloudy",&weather4},//4
    {"Partly Cloudy",&weather5},//5
    {"Partly Cloudy",&weather6},//6
    {"Mostly Cloudy",&weather5},//7
    {"Mostly Cloudy",&weather6},//8
    {"Overcast",&weather4},//9
    {"Shower",&weather10},//10
    {"Thunder Shower",&weather11},//11
    {"Thunderstorm with Hail",&weather11},//12
    {"Light Rain",&weather13},//13
    {"Moderate Rain",&weather14},//14
    {"Heavy Rain",&weather16},//15
    {"Rainstorm",&weather16},//16
    {"Heavy Rainstorm",&weather15},//17
    {"Severe Rainstorm",&weather15},//18
    {"Freezing Rain",&weather19},//19
    {"Sleet",&weather20},//20
    {"Snow Shower",&weather21},//21
    {"Light Snow",&weather22},//22
    {"Moderate Snow",&weather23},//23
    {"Heavy Snow",&weather24},//24
    {"Snowstorm",&weather24},//25
    {"Dust",&weather26},//26
    {"Sand",&weather26},//27
    {"Sandstorm",&weather28},//28
    {"Severe Sandstorm",&weather28},//29
    {"Fog",&weather30},//30
    {"Haze",&weather32},//31
    {"Windy",&weather32},//32
    {"Strong Wind",&weather32},//33
    {"Hurricane",&weather32},//34
    {"Tropical Storm",&weather35},//35
    {"Tornado",&weather36},//36
    {"Cold",&weather40},//37
    {"Hot",&weather40},//38
    {"Unknown",&weather40},//39
};
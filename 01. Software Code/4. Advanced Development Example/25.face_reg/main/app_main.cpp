#include "who_camera.h"
#include "who_lcd.h"
#include "who_adc_button.h"
#include "who_button.h"
#include "who_color_detection.hpp"
#include "who_human_face_detection.hpp"
#include "who_human_face_recognition.hpp"
#include "who_cat_face_detection.hpp"
#include "who_motion_detection.hpp"
#include "app_wifi.h"
#include "app_httpd.hpp"
#include "app_mdns.h"
#include "event_logic.hpp"
static QueueHandle_t xQueueHttpFrame = NULL;
static QueueHandle_t xQueueAIFrame = NULL;
static QueueHandle_t xQueueLCDFrame = NULL;
static QueueHandle_t xQueueKeyState = NULL;
static QueueHandle_t xQueueADCKeyState = NULL;
static QueueHandle_t xQueueGPIOKeyState = NULL;
static QueueHandle_t xQueueEventLogic = NULL;

        // {0, 2500, 2800}, //middle button  2.25v
        // {1, 1150, 1350}, //right click 1.12v
        // {2, 1800, 2300}, //left click  1.689v
        // {3, 500, 750}, //Down button 0.562v
        // {4, 3200, 3600}, //up key 2.81v
static button_adc_config_t buttons[5] = {{1, 2500, 2800}, {2, 1150, 1350}, {3, 1800, 2300}, {4, 500, 750}, {5, 3200, 3600}};

/*Human face web-end recognition*/
// extern "C" void app_main()
// {
//     app_wifi_main();
//     app_mdns_main();

//     xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
//     xQueueHttpFrame = xQueueCreate(2, sizeof(camera_fb_t *));

//     register_camera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueAIFrame);
//     register_human_face_detection(xQueueAIFrame, NULL, NULL, xQueueHttpFrame);
//     register_httpd(xQueueHttpFrame, NULL, true);
// }
/*Human face detection, LCD display*/
// extern "C" void app_main()
// {
//     xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
//     xQueueLCDFrame = xQueueCreate(2, sizeof(camera_fb_t *));

//     register_camera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueAIFrame);
//     sensor_t *s = esp_camera_sensor_get();
    //  // s->set_vflip(s, 1); //Invert image
//     register_human_face_detection(xQueueAIFrame, NULL, NULL, xQueueLCDFrame, false);
//     register_lcd(xQueueLCDFrame, NULL, true);
// }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Human face recognition, LCD display*/
// #define GPIO_BOOT GPIO_NUM_0

// extern "C" void app_main()
// {
//     xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
//     xQueueLCDFrame = xQueueCreate(2, sizeof(camera_fb_t *));
//     xQueueKeyState = xQueueCreate(1, sizeof(int *));
//     xQueueEventLogic = xQueueCreate(1, sizeof(int *));

//     // register_button(GPIO_BOOT, xQueueKeyState);
//     register_camera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueAIFrame);
//     sensor_t *s = esp_camera_sensor_get();
//     s->set_vflip(s, 1); //Invert image
//     s->set_hmirror(s, 1); //Invert image
//     register_adc_button(buttons, 5, xQueueKeyState);
//     register_event(xQueueKeyState, xQueueEventLogic);
//     register_human_face_recognition(xQueueAIFrame, xQueueEventLogic, NULL, xQueueLCDFrame, false);
//     register_lcd(xQueueLCDFrame, NULL, true);
// }

/*Color recognition, LCD display*/
// #define GPIO_BOOT GPIO_NUM_0

// extern "C" void app_main()
// {
//     gpio_config_t gpio_conf;
//     gpio_conf.mode = GPIO_MODE_OUTPUT_OD;
//     gpio_conf.intr_type = GPIO_INTR_DISABLE;
//     gpio_conf.pin_bit_mask = 1LL << GPIO_NUM_3;
//     gpio_config(&gpio_conf);
    
//     xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
//     xQueueLCDFrame = xQueueCreate(2, sizeof(camera_fb_t *));
//     xQueueADCKeyState = xQueueCreate(1, sizeof(int));
//     xQueueGPIOKeyState = xQueueCreate(1, sizeof(int));
//     xQueueEventLogic = xQueueCreate(1, sizeof(int));

//     register_camera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueAIFrame);
//     register_adc_button(buttons, 5, xQueueADCKeyState);
//     register_button(GPIO_NUM_0, xQueueGPIOKeyState);
//     // register_event(xQueueADCKeyState, xQueueGPIOKeyState, xQueueEventLogic);
//     register_color_detection(xQueueAIFrame, xQueueEventLogic, NULL, xQueueLCDFrame, false);
//     register_lcd(xQueueLCDFrame, NULL, true);
// }

/*Cat face detection LCD display*/
extern "C" void app_main()
{
    xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
    xQueueLCDFrame = xQueueCreate(2, sizeof(camera_fb_t *));

    register_camera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueAIFrame);
    register_cat_face_detection(xQueueAIFrame, NULL, NULL, xQueueLCDFrame, false);
    register_lcd(xQueueLCDFrame, NULL, true);

}

/*Motion detection, LCD display*/
// extern "C" void app_main()
// {
//     xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
//     xQueueLCDFrame = xQueueCreate(2, sizeof(camera_fb_t *));

//     register_camera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueAIFrame);
//     register_motion_detection(xQueueAIFrame, NULL, NULL, xQueueLCDFrame);
//     register_lcd(xQueueLCDFrame, NULL, true);
// }
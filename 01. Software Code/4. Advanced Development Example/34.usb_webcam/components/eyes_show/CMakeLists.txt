idf_component_register(
    SRCS "img_blink_eyes.c" "img_close_eyes.c" "img_open_eyes.c" "img_static_eyes.c" "show_eyes.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES fatfs esp_lcd lvgl__lvgl   espressif__esp32_s3_eye
)
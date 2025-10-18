idf_component_register(
    SRCS "esp32_s3_eye.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "priv_include"
    REQUIRES driver espressif__button
    PRIV_REQUIRES fatfs esp_lcd lvgl__lvgl   espressif__esp_lvgl_port
)
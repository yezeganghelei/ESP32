set(COMPONENT_SRCS 
    "app_main.c"
    "page_imu.c"
    "app_camera.c"
    "app_led.c"
    "button/button.c"
    "button/bsp_button.c"
    "page/page_wakeup.c"
    "app_speech_recsrc.c"
    "app_speech_wakeup.c"
    "file_manager.c"
    "wav_player.c"
    )
set(COMPONENT_ADD_INCLUDEDIRS  "." "button"  "page")

register_component()
spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)
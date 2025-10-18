set(COMPONENT_SRCS 
    "app_main.c"
    "screen/screen.c"
    "screen/fontx.c"
    "screen/pngle.c"
    "app_camera.c"

    )
set(COMPONENT_ADD_INCLUDEDIRS "." "screen")

register_component()
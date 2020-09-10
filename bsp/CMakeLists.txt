
if(WIN32)
    set(BSP_THREAD  bsp_thread_windows.c)
else()
    set(BSP_THREAD  bsp_thread_pthread.c)
endif()

add_library(bsp_config_1
    bsp_temperature_a.c
    bsp_led.c
    bsp_exception.c
    ${BSP_THREAD}
)

add_library(bsp_config_2
    bsp_temperature_b.c
    bsp_led.c
    bsp_exception.c
    ${BSP_THREAD}
)

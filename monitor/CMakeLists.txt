add_library(monitor
    monitor.c
)

target_include_directories(monitor 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/bsp
        ${CMAKE_SOURCE_DIR}/monitor
)


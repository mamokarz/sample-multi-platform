add_library(monitor
    monitor.c
)

target_include_directories(monitor 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/pal
        ${CMAKE_SOURCE_DIR}/monitor
)


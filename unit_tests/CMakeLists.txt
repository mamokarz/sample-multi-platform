
add_executable(temperature_reader_ut
    temperature_reader_ut.c
)

target_include_directories(temperature_reader_ut 
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/bsp
        ${CMAKE_SOURCE_DIR}/monitor
)

target_link_libraries(temperature_reader_ut 
    PRIVATE
        monitor
        bsp_config_1
)



function(add_sample config_name)
    set(TARGET_NAME "sample_${config_name}")
    add_executable(${TARGET_NAME} 
        main.c
    )

    target_include_directories(${TARGET_NAME} 
        PUBLIC 
            ${CMAKE_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/bsp
            ${CMAKE_SOURCE_DIR}/monitor
    )

    target_link_libraries(${TARGET_NAME} 
        PUBLIC 
            monitor
            ${config_name}
    )
endfunction()
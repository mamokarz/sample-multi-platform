# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
# See LICENSE file in the project root for full license information.

##
# Define the PAL version
##
set(PAL_VERSION_MAJOR 0)
set(PAL_VERSION_MINOR 1)
set(PAL_VERSION_PATCH 0)

##
# Description of PAL configurations for Windows.
##
list(APPEND pal_win_1
    pal_version.c
    pal_temperature_a.c
    pal_led.c
    pal_fan_a.c
    pal_exception.c
    pal_thread_windows.c
)

list(APPEND pal_win_2
    pal_version.c
    pal_temperature_b.c
    pal_led.c
    pal_fan_b.c
    pal_exception.c
    pal_thread_windows.c
)

list(APPEND pal_win_3
    pal_version.c
    pal_temperature_b.c
    pal_led.c
    pal_fan_c.c
    pal_exception.c
    pal_thread_windows.c
)

##
# Description of PAL configurations for Linux.
##
list(APPEND pal_linux_1
    pal_version.c
    pal_temperature_a.c
    pal_led.c
    pal_fan_a.c
    pal_exception.c
    pal_thread_pthread.c
)

list(APPEND pal_linux_2
    pal_version.c
    pal_temperature_b.c
    pal_led.c
    pal_fan_b.c
    pal_exception.c
    pal_thread_pthread.c
)

list(APPEND pal_linux_3
    pal_version.c
    pal_temperature_b.c
    pal_led.c
    pal_fan_c.c
    pal_exception.c
    pal_thread_pthread.c
)

##
# List with the configurations that shall generate PAL libraries and shall be tested.
# Make decision o each set of pal shall be tested based on the OS and compiler.
##
if(WIN32)
    list(APPEND pal_libraries 
        pal_win_1 
        pal_win_2
        pal_win_3
    )
else()
    list(APPEND pal_libraries 
        pal_linux_1 
        pal_linux_2
        pal_linux_3
    )
endif()


##
# Function to create a device for the library.
##
function(add_device config_name)
    set(TARGET_NAME "device_${config_name}")
    message(STATUS "Creating device ${TARGET_NAME}")
    add_executable(${TARGET_NAME} 
        main.c
    )

    target_include_directories(${TARGET_NAME} 
        PUBLIC 
            ${CMAKE_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/pal
            ${CMAKE_SOURCE_DIR}/monitor
    )

    target_link_libraries(${TARGET_NAME} 
        PUBLIC 
            monitor
            ${config_name}
    )
endfunction()

##
# Function to create a compliance test for the library.
##
function(add_compliance_test config_name)
    set(TARGET_NAME "${config_name}_compliance_ut")
    message(STATUS "Creating compliance test ${TARGET_NAME}")
    add_executable(${TARGET_NAME} 
        pal_compliance_ut.c
    )

    target_include_directories(${TARGET_NAME} 
        PUBLIC 
            ${CMAKE_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/pal
    )

    target_link_libraries(${TARGET_NAME} 
        PUBLIC 
            ${config_name}
    )
endfunction()

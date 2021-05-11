# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
# See LICENSE file in the project root for full license information.

add_executable(temperature_monitor_ut
    temperature_monitor_ut.c
)

target_include_directories(temperature_monitor_ut 
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/pal
        ${CMAKE_SOURCE_DIR}/monitor
)

list (GET pal_libraries 0 pal)

target_link_libraries(temperature_monitor_ut 
    PRIVATE
        monitor
        ${pal}
)

foreach(pal IN LISTS pal_libraries)
    add_compliance_test(${pal})
endforeach()

# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
# See LICENSE file in the project root for full license information.

add_library(monitor
    ${CMAKE_CURRENT_LIST_DIR}/monitor.c
)

target_include_directories(monitor
    PUBLIC
        ${temperature_monitor_SOURCE_DIR}/pal
        ${temperature_monitor_SOURCE_DIR}/monitor
)


cmake_minimum_required(VERSION 3.10)

project(temperature_monitor VERSION 0.0.1)

set(CMAKE_VERBOSE_MAKEFILE off)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(config)

add_subdirectory(pal)
add_subdirectory(monitor)
add_subdirectory(tests)
add_subdirectory(device)

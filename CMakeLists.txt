
cmake_minimum_required(VERSION 3.15)

project(temperature_reader VERSION 0.0.1)

set(CMAKE_VERBOSE_MAKEFILE on)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(config)

add_subdirectory(bsp)
add_subdirectory(monitor)
add_subdirectory(unit_tests)

add_sample(bsp_config_1)
add_sample(bsp_config_2)

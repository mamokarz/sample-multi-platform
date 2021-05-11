# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
# See LICENSE file in the project root for full license information.

##
# Create all libraries in the pal_libraries list for the chosen platform.
##
foreach(pal IN LISTS pal_libraries)
    message(STATUS "creating library ${pal}[${${pal}}]")
    add_library(${pal}
        ${${pal}}
    )
    target_compile_definitions(${pal} PUBLIC
        "PAL_VERSION_MAJOR=${PAL_VERSION_MAJOR}"
        "PAL_VERSION_MINOR=${PAL_VERSION_MINOR}"
        "PAL_VERSION_PATCH=${PAL_VERSION_PATCH}")
endforeach()


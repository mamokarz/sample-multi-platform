# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
# See LICENSE file in the project root for full license information.

foreach(pal IN LISTS pal_libraries)
    add_device(${pal})
endforeach()

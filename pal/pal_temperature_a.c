// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.
// See LICENSE file in the project root for full license information.

#include <stddef.h>
#include <stdio.h>
#include "pal.h"

pal_result pal_temperature_get(unsigned short* temperature) {
    printf("This is the real pal_temperature_get for device A\r\n");
    if (temperature == NULL) return PAL_ERROR_ARG;
    *temperature = 65;
    return PAL_OK;
}

float pal_temperature_to_celsius(unsigned short temperature) {
    return ((float)(temperature - 40));
}


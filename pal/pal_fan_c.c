// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.
// See LICENSE file in the project root for full license information.

#include <stdio.h>
#include "pal.h"

pal_result pal_fan_set_speed(int speed_rpm) {
    printf("This is the real pal_fan_set_speed C\r\n");

    if ((speed_rpm > 4500) || (speed_rpm < 0)) {
        return PAL_ERROR_ARG;
    }

    printf("Set speed to %d\r\n", speed_rpm);

    return PAL_OK;
}


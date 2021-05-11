// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.
// See LICENSE file in the project root for full license information.

#include "pal.h"
#include <stdio.h>

/** The pal_temperature_get shall return the current temperature and PAL_OK. */
int pal_temperature_get_success(void) {
    printf("Execute test pal_temperature_get_success\r\n");
    ///arrange
    unsigned short temperature = 0xFFFF;

    ///act
    pal_result result = pal_temperature_get(&temperature);

    ///assert
    if (result != PAL_OK) {
        printf("pal_temperature_get return failed. Expected:PAL_OK Actual:%u\r\n\r\n", result);
        return 1;
    }
    if (temperature == 0xFFFF) {
        printf("pal_temperature_get temperature failed. Expected:different then 0xFF Actual:0xFF\r\n\r\n");
        return 1;
    }
    printf("Succeeded\r\n\r\n");
    return 0;
}

/** The pal_temperature_get shall return PAL_ERROR_ARG if the temperature pointer is NULL. */
int pal_temperature_get_with_null_pointer_fail(void) {
    printf("Execute test pal_temperature_get_with_null_pointer_fail\r\n");
    ///arrange

    ///act
    pal_result result = pal_temperature_get(NULL);

    ///assert
    if (result != PAL_ERROR_ARG) {
        printf("pal_temperature_get return failed. Expected:PAL_ERROR_ARG Actual:%u\r\n\r\n", result);
        return 1;
    }
    printf("Succeeded\r\n\r\n");
    return 0;
}

/** The pal_fan_set_speed shall return PAL_OK for speeds between 1200 and -800. */
int pal_fan_set_speed_hi_success(void) {
    printf("Execute test pal_fan_set_speed_hi_success\r\n");
    ///arrange
    int speed_rpm = 1200;

    ///act
    pal_result result = pal_fan_set_speed(speed_rpm);

    ///assert
    if (result != PAL_OK) {
        printf("pal_fan_set_speed return failed. Expected:PAL_OK Actual:%u\r\n\r\n", result);
        return 1;
    }
    printf("Succeeded\r\n\r\n");
    return 0;
}

int pal_fan_set_speed_low_success(void) {
    printf("Execute test pal_fan_set_speed_low_success\r\n");
    ///arrange
    int speed_rpm = -800;

    ///act
    pal_result result = pal_fan_set_speed(speed_rpm);

    ///assert
    if (result != PAL_OK) {
        printf("pal_fan_set_speed return failed. Expected:PAL_OK Actual:%u\r\n\r\n", result);
        return 1;
    }
    printf("Succeeded\r\n\r\n");
    return 0;
}

/**
 * Main test system.
 */
int main(void)
{
    unsigned int count_error = 0;

    printf("Starting unit tests:\r\n");

    if (pal_temperature_get_success() != 0) count_error++;
    if (pal_temperature_get_with_null_pointer_fail() != 0) count_error++;
    if (pal_fan_set_speed_hi_success() != 0) count_error++;
    if (pal_fan_set_speed_low_success() != 0) count_error++;

    printf("End unit tests with %u errors\r\n", count_error);

    return count_error;
}



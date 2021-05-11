// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.
// See LICENSE file in the project root for full license information.

#include "pal.h"

#define NUMBER_SAMPLES   3
#define SAMPLE_INTERVAL  1000

void temperature_monitor(void) {
    unsigned short t;
    float t_c = 0.0;
    pal_result result = PAL_ERROR_ARG;
    for (int i = 0; i < NUMBER_SAMPLES; i++)
    {
        result = pal_temperature_get(&t);
        if (result == PAL_OK) {
            t_c += pal_temperature_to_celsius(t);
        }
        else if (result == PAL_ERROR_ARG) {
            pal_halt();
        }

        if ((i + 1) < NUMBER_SAMPLES)
            pal_sleep(SAMPLE_INTERVAL);
    }

    t_c /= (float)NUMBER_SAMPLES;

    if (result == PAL_OK)
    {
        if (t_c > 120.0) {
            pal_led_set_color(PAL_LED_RED);
            pal_fan_set_speed((int)(0.0f));
            pal_halt();
            t_c = 0;
        }
        else {
            if ((t_c > 80.0) || (t_c < -38.0)) {
                pal_led_set_color(PAL_LED_RED);
            }
            else if ((t_c > 60.0) || (t_c < -10.0)) {
                pal_led_set_color(PAL_LED_YELLOW);
            }
            else {
                pal_led_set_color(PAL_LED_GREEN);
            }
            pal_fan_set_speed((int)(t_c * 10.0f));
        }
    }
    else {
        pal_led_set_color(PAL_LED_RED);
        pal_halt();
    }
}


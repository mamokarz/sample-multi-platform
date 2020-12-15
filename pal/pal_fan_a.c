#include <stdio.h>
#include "pal.h"

pal_result pal_fan_set_speed(int speed_rpm){
    printf("This is the real pal_fan_set_speed A\r\n");

    if((speed_rpm > 1500) || (speed_rpm < -1000)) {
        return PAL_ERROR_ARG;
    }

    printf("Set speed to %d\r\n", speed_rpm);

    return PAL_OK;
}


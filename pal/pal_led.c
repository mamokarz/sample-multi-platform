#include <stdio.h>
#include "pal.h"

pal_result pal_led_set_color(pal_led_color color){
    printf("This is the real pal_led_set_color\r\n");
    switch(color) {
        case PAL_LED_RED:    printf("LED: RED\r\n"); break;
        case PAL_LED_YELLOW: printf("LED: YELLOW\r\n"); break;
        case PAL_LED_GREEN:  printf("LED: GREEN\r\n"); break;
        default:             return PAL_ERROR_ARG;
    }
    return PAL_OK;
}


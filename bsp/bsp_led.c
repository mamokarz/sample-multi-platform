#include <stdio.h>
#include "bsp.h"

bsp_result bsp_led_set_color(bsp_led_color color){
    printf("This is the real bsp_led_set_color\r\n");
    switch(color) {
        case BSP_LED_RED:    printf("LED: RED\r\n"); break;
        case BSP_LED_YELLOW: printf("LED: YELLOW\r\n"); break;
        case BSP_LED_GREEN:  printf("LED: GREEN\r\n"); break;
        default:             return BSP_ERROR_ARG;
    }
    return BSP_OK;
}


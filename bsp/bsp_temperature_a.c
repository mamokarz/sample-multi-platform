#include <stddef.h>
#include <stdio.h>
#include "bsp.h"

bsp_result bsp_temperature_get(unsigned char* temperature) {
    printf("This is the real bsp_temperature_get for device A\r\n");
    if(temperature == NULL) return BSP_ERROR_ARG;
    *temperature = 65;
    return BSP_OK;
}

float bsp_temperature_to_fahrenheit(unsigned char temperature) {
    return (((float)(temperature - 40)) * 1.8) + 32.0;
}

float bsp_temperature_to_celsius(unsigned char temperature) {
    return ((float)(temperature - 40));
}


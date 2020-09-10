#include <stdio.h>
#include <stddef.h>
#include "bsp.h"
#include "monitor.h"

int main()
{
    unsigned char temperature;

    bsp_result result = bsp_temperature_get(&temperature);
    if(result != BSP_OK) return (int)result;

    float t_c = bsp_temperature_to_celsius(temperature);
    float t_f = bsp_temperature_to_fahrenheit(temperature);

    printf("My current temperature is %.1fc or %.1ff\r\n", t_c, t_f);

    while(1) {
        temperature_allert();
        bsp_sleep(1000);
    }

    return 0;
}


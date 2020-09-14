#include <stdio.h>
#include <stddef.h>
#include "pal.h"
#include "monitor.h"

int main()
{
    unsigned short temperature;

    pal_result result = pal_temperature_get(&temperature);
    if(result != PAL_OK) return (int)result;

    float t_c = pal_temperature_to_celsius(temperature);

    printf("My current PAL version is %s\r\n", pal_version_get_string());

    printf("My current temperature is %.1fc\r\n", t_c);

    while(1) {
        temperature_monitor();
        pal_sleep(1000);
    }

    return 0;
}


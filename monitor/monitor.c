#include "bsp.h"

#define NUMBER_SAMPLES   3
#define SAMPLE_INTERVAL  1000

void temperature_allert(void) {
    unsigned char t;
    float t_c = 0.0;
    bsp_result result = BSP_ERROR_ARG; 
    for(int i = 0; i < NUMBER_SAMPLES; i++)
    {
        result = bsp_temperature_get(&t);
        if(result == BSP_OK) {
            t_c += bsp_temperature_to_celsius(t);
        } else if(result == BSP_ERROR_ARG) {
            bsp_halt();
        }

        if((i + 1) < NUMBER_SAMPLES)
            bsp_sleep(SAMPLE_INTERVAL);
    }

    t_c /= (float)NUMBER_SAMPLES;
    
    if(result == BSP_OK)
    {    
        if(t_c > 120.0) {
            bsp_led_set_color(BSP_LED_RED);
            bsp_halt();
        } else if((t_c > 80.0) || (t_c < -38.0)) {
            bsp_led_set_color(BSP_LED_RED);
        } else if((t_c > 60.0) || (t_c < -10.0)) {
            bsp_led_set_color(BSP_LED_YELLOW);
        } else {
            bsp_led_set_color(BSP_LED_GREEN);
        }
    } else {
        bsp_led_set_color(BSP_LED_YELLOW);
    }
}


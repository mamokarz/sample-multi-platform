#include "bsp.h"
#include "monitor.h"
#include <stdio.h>

/**
 * Session of mock functions that will replace the BSP ones.
 */
static bsp_led_color g_led_color;
static bsp_result g_bsp_led_set_color_result;
bsp_result bsp_led_set_color(bsp_led_color color) 
{
    printf("This is the mock bsp_led_set_color\r\n");
    g_led_color = color;
    return g_bsp_led_set_color_result;
}

static unsigned char g_temperature;
static bsp_result g_bsp_temperature_get_result;
bsp_result bsp_temperature_get(unsigned char* temperature)
{
    printf("This is the mock bsp_temperature_get\r\n");
    *temperature = g_temperature;
    return g_bsp_temperature_get_result;
}

float bsp_temperature_to_fahrenheit(unsigned char temperature) {
    return (((float)(temperature - 80) / 2.0) * 1.8) + 32.0;
}

float bsp_temperature_to_celsius(unsigned char temperature) {
    return ((float)(temperature - 80) / 2.0);
}

/** The temperature_allert shall turn the LED green if the temperature is between -10.0c and 60.0c */
int temperature_allert_with_green_value_success(void) {
    printf("Execute test temperature_allert_with_green_value_success\r\n");
    ///arrange
    g_temperature = 90;
    g_bsp_temperature_get_result = BSP_OK;
    g_led_color = BSP_LED_OFF;
    g_bsp_led_set_color_result = BSP_OK;

    ///act
    temperature_allert();

    ///assert
    if(g_led_color != BSP_LED_GREEN) {
        printf("g_led_color failed. Expected:BSP_LED_GREEN Actual:%u\r\n\r\n", g_led_color);
        return 1;
    }
    printf("Succeeded\r\n\r\n");
    return 0; 
}

/** The temperature_allert shall turn the LED yellow if the temperature is between -38.0c and -10.0c or between 80.0c and 60.0c */
int temperature_allert_with_yellow_value_success(void) {
    printf("Execute test temperature_allert_with_yellow_value_success\r\n");
    ///arrange
    g_temperature = (unsigned char)210;
    g_bsp_temperature_get_result = BSP_OK;
    g_led_color = BSP_LED_OFF;
    g_bsp_led_set_color_result = BSP_OK;

    ///act
    temperature_allert();

    ///assert
    if(g_led_color != BSP_LED_YELLOW) {
        printf("g_led_color failed. Expected:BSP_LED_YELLOW Actula:%u\r\n\r\n", g_led_color);
        return 1;
    }
    printf("Succeeded\r\n\r\n");
    return 0; 
}

/** The temperature_allert shall turn the LED red if the temperature is between -38.0c and -10.0c or between 80.0c and 60.0c */
int temperature_allert_with_red_value_success(void) {
    printf("Execute test temperature_allert_with_red_value_success\r\n");
    ///arrange
    g_temperature = (unsigned char)250;
    g_bsp_temperature_get_result = BSP_OK;
    g_led_color = BSP_LED_OFF;
    g_bsp_led_set_color_result = BSP_OK;

    ///act
    temperature_allert();

    ///assert
    if(g_led_color != BSP_LED_RED) {
        printf("g_led_color failed. Expected:BSP_LED_RED Actula:%u\r\n\r\n", g_led_color);
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

    if(temperature_allert_with_green_value_success() != 0) count_error++;
    if(temperature_allert_with_yellow_value_success() != 0) count_error++;
    if(temperature_allert_with_red_value_success() != 0) count_error++;

    printf("End unit tests with %u errors\r\n", count_error);

    return count_error;
}



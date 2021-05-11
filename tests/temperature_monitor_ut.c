// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.
// See LICENSE file in the project root for full license information.

#include "pal.h"
#include "monitor.h"
#include <stdio.h>
#include <stdbool.h>

/**
 * Session of mock functions that will replace the PAL ones.
 */
static pal_led_color g_led_color;
static pal_result g_pal_led_set_color_result;
pal_result pal_led_set_color(pal_led_color color)
{
    printf("This is the mock pal_led_set_color\r\n");
    g_led_color = color;
    return g_pal_led_set_color_result;
}

static unsigned short g_temperature;
static pal_result g_pal_temperature_get_result;
pal_result pal_temperature_get(unsigned short* temperature)
{
    printf("This is the mock pal_temperature_get\r\n");
    *temperature = g_temperature;
    return g_pal_temperature_get_result;
}

float pal_temperature_to_celsius(unsigned short temperature) {
    return ((float)(temperature - 80) / 2.0f);
}

static int g_speed_rpm;
static pal_result g_pal_fan_set_speed_result;
pal_result pal_fan_set_speed(int speed_rpm) {
    printf("This is the mock pal_fan_set_speed\r\n");
    g_speed_rpm = speed_rpm;
    return g_pal_fan_set_speed_result;
}

static bool g_pal_halt;
void pal_halt(void) {
    printf("This is the mock pal_halt... HALT!\r\n");
    g_pal_halt = true;
}

void pre_test(void) {
    g_led_color = PAL_LED_OFF;
    g_pal_led_set_color_result = PAL_OK;
    g_temperature = 80;
    g_pal_temperature_get_result = PAL_OK;
    g_speed_rpm = 0;
    g_pal_fan_set_speed_result = PAL_OK;
    g_pal_halt = false;
}

void post_test(void) {

}

/**
 * Helper macro to execute tests.
 */
#define RUN_TEST(test, count_error) \
do { \
    pre_test();\
    if(test() != 0) count_error++; \
    post_test(); \
} while(false);

 /** temperature_monitor shall set the fan speed to 10 times the reported temperature. */
 /** temperature_monitor shall turn the LED green if the temperature is between -10.0c and 60.0c */
int temperature_monitor_with_green_value_success(void) {
    printf("Execute test temperature_monitor_with_green_value_success\r\n");
    ///arrange
    g_temperature = 90;

    ///act
    temperature_monitor();

    ///assert
    if (g_led_color != PAL_LED_GREEN) {
        printf("g_led_color failed. Expected:PAL_LED_GREEN Actual:%u\r\n\r\n", g_led_color);
        return 1;
    }
    if (g_speed_rpm != 50) {
        printf("g_speed_rpm failed. Expected:50 Actual:%u\r\n\r\n", g_speed_rpm);
        return 1;
    }
    if (g_pal_halt != false) {
        printf("System halted\r\n\r\n");
        return 1;
    }
    printf("Succeeded\r\n\r\n");
    return 0;
}

/** temperature_monitor shall turn the LED yellow if the temperature is between -38.0c and -10.0c or between 80.0c and 60.0c */
int temperature_monitor_with_yellow_value_success(void) {
    printf("Execute test temperature_monitor_with_yellow_value_success\r\n");
    ///arrange
    g_temperature = 210;

    ///act
    temperature_monitor();

    ///assert
    if (g_led_color != PAL_LED_YELLOW) {
        printf("g_led_color failed. Expected:PAL_LED_YELLOW Actual:%u\r\n\r\n", g_led_color);
        return 1;
    }
    if (g_speed_rpm != 650) {
        printf("g_speed_rpm failed. Expected:650 Actual:%u\r\n\r\n", g_speed_rpm);
        return 1;
    }
    if (g_pal_halt != false) {
        printf("System halted\r\n\r\n");
        return 1;
    }
    printf("Succeeded\r\n\r\n");
    return 0;
}

/** temperature_monitor shall turn the LED red if the temperature is between -38.0c and -10.0c or between 80.0c and 60.0c */
int temperature_monitor_with_red_value_success(void) {
    printf("Execute test temperature_monitor_with_red_value_success\r\n");
    ///arrange
    g_temperature = 250;

    ///act
    temperature_monitor();

    ///assert
    if (g_led_color != PAL_LED_RED) {
        printf("g_led_color failed. Expected:PAL_LED_RED Actual:%u\r\n\r\n", g_led_color);
        return 1;
    }
    if (g_speed_rpm != 850) {
        printf("g_speed_rpm failed. Expected:850 Actual:%u\r\n\r\n", g_speed_rpm);
        return 1;
    }
    if (g_pal_halt != false) {
        printf("System halted\r\n\r\n");
        return 1;
    }
    printf("Succeeded\r\n\r\n");
    return 0;
}

/** temperature_monitor shall set LED to red, turn fan off, and halt the system if the temperature if above 120.0c or if the temperature sensor failed to read the temperature. */
int temperature_monitor_with_too_hight_temperature_halt(void) {
    printf("Execute test temperature_monitor_with_too_hight_temperature_halt\r\n");
    ///arrange
    g_temperature = 330;

    ///act
    temperature_monitor();

    ///assert
    if (g_led_color != PAL_LED_RED) {
        printf("g_led_color failed. Expected:PAL_LED_RED Actual:%u\r\n\r\n", g_led_color);
        return 1;
    }
    if (g_speed_rpm != 0) {
        printf("g_speed_rpm failed. Expected:0 Actual:%u\r\n\r\n", g_speed_rpm);
        return 1;
    }
    if (g_pal_halt != true) {
        printf("System didn't halt\r\n\r\n");
        return 1;
    }
    printf("Succeeded\r\n\r\n");
    return 0;
}

int temperature_monitor_with_temperature_sensor_error_halt(void) {
    printf("Execute test temperature_monitor_with_temperature_sensor_error_halt\r\n");
    ///arrange
    g_pal_temperature_get_result = PAL_ERROR_UNAVAILABLE;

    ///act
    temperature_monitor();

    ///assert
    if (g_led_color != PAL_LED_RED) {
        printf("g_led_color failed. Expected:PAL_LED_RED Actual:%u\r\n\r\n", g_led_color);
        return 1;
    }
    if (g_speed_rpm != 0) {
        printf("g_speed_rpm failed. Expected:0 Actual:%u\r\n\r\n", g_speed_rpm);
        return 1;
    }
    if (g_pal_halt != true) {
        printf("System didn't halt\r\n\r\n");
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

    RUN_TEST(temperature_monitor_with_green_value_success, count_error);
    RUN_TEST(temperature_monitor_with_yellow_value_success, count_error);
    RUN_TEST(temperature_monitor_with_red_value_success, count_error);
    RUN_TEST(temperature_monitor_with_too_hight_temperature_halt, count_error);
    RUN_TEST(temperature_monitor_with_temperature_sensor_error_halt, count_error);

    printf("End unit tests with %u errors\r\n", count_error);

    return count_error;
}




#ifndef PAL_H
#define PAL_H

typedef enum {
    PAL_OK,
    PAL_ERROR_OUT_OF_MEMORY,
    PAL_ERROR_ARG,
    PAL_ERROR_UNAVAILABLE
} pal_result;

typedef enum {
    PAL_LED_OFF,
    PAL_LED_RED,
    PAL_LED_YELLOW,
    PAL_LED_GREEN
} pal_led_color;

/**
 * @brief  Get a string with the current pal version.
 *
 * @return The current PAL version.
 */
const char* pal_version_get_string(void);

/**
 * @brief  Get an integer with the major of the current pal version.
 *
 * @return The major of the current PAL version.
 */
const int pal_version_get_major(void);

/**
 * @brief  Get an integer with the minor of the current pal version.
 *
 * @return The minor of the current PAL version.
 */
const int pal_version_get_minor(void);

/**
 * @brief  Get an integer with the patch of the current pal version.
 *
 * @return The patch of the current PAL version.
 */
const int pal_version_get_patch(void);

/**
 * @brief  Get the current temperature on the MCU
 *
 * @param[out] temperature      The pointer to 'unsigned short' to return the current temperature in a scale from 0 to 65535.
 *
 * @return The #pal_result with the operation result.
 *      @retval  #PAL_OK                    If the operation succeeded and the temperature can be used.
 *      @retval  #PAL_ERROR_ARG             If the provided argument is invalid.
 *      @retval  #PAL_ERROR_UNAVAILABLE     If there is no temperature sensor.
 */
pal_result pal_temperature_get(unsigned short* temperature);

/**
 * @brief  Convert temperature to Celsius 
 *
 * @param[in]  temperature      The 'unsigned short' with the temperature in a scale from 0 to 65535.
 *
 * @return The temperature converted in Celsius.
 */
float pal_temperature_to_celsius(unsigned short temperature);

/**
 * @brief  Change the LED color
 *
 * @param[in]  color            The #pal_led_color with the new color of the LED.
 *
 * @return The #pal_result with the operation result.
 *      @retval  #PAL_OK                    If the operation succeeded and the LED has the new color.
 *      @retval  #PAL_ERROR_ARG             If the provided argument is invalid.
 *      @retval  #PAL_ERROR_UNAVAILABLE     If there is no LED on the board.
 */
pal_result pal_led_set_color(pal_led_color color);

/**
 * @brief  Change the fan speed
 *
 * @param[in]  speed_rpm        The `int` with the new speed in RPM.
 *                                  0   fan not running
 *                                  +n  fan running clockwise
 *                                  -n  fun running anticlockwise
 *
 * @return The #pal_result with the operation result.
 *      @retval  #PAL_OK                    If the operation succeeded and the fan is running in the new speed.
 *      @retval  #PAL_ERROR_ARG             If the provided argument is invalid.
 *      @retval  #PAL_ERROR_UNAVAILABLE     If there is no available fan in the board.
 */
pal_result pal_fan_set_speed(int speed_rpm);

/**
 * @brief  Generate a hard exception that will halt the processor.
 */
void pal_halt(void);

/**
 * @brief  Suspend the process for time_us microseconds.
 *
 * @param[in]  sleep_time_ms    Time to sleep the process in milliseconds.
 */
void pal_sleep(unsigned int sleep_time_ms);

#endif //PAL_H

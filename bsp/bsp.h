
typedef enum {
    BSP_OK,
    BSP_ERROR_OUT_OF_MEMORY,
    BSP_ERROR_ARG,
    BSP_ERROR_UNABAILABLE
} bsp_result;

typedef enum {
    BSP_LED_OFF,
    BSP_LED_RED,
    BSP_LED_YELLOW,
    BSP_LED_GREEN
} bsp_led_color;

/**
 * @brief  Get the current temperature on the MCU
 *
 * @param[out] temperature    The pointer to 'unsigned char' to return the current temperature in a scale from 0 to 255.
 *
 * @return The 'bsp_result' with the operation result.
 *      @retval  BSP_OK                 If the operation succeeded and the temperature can be used.
 *      @retval  BSP_ERROR_ARG          If the provided argument is invalid.
 *      @retval  BSP_ERROR_UNAVAILABLE  If there is no temperature sensor.
 */
bsp_result bsp_temperature_get(unsigned char* temperature);

/**
 * @brief  Convert temperature to Fahrenheits
 *
 * @param[in]  temperature    The 'unsigned char' with the temperature in a scale from 0 to 255.
 *
 * @return The temperature converted in Fahrenheits.
 */
float bsp_temperature_to_fahrenheit(unsigned char temperature);

/**
 * @brief  Convert temperature to Celsius 
 *
 * @param[in]  temperature    The 'unsigned char' with the temperature in a scale from 0 to 255.
 *
 * @return The temperature converted in Celsius.
 */
float bsp_temperature_to_celsius(unsigned char temperature);

/**
 * @brief  Change the LED color
 *
 * @param[in]  color    The #bsp_led_color with the new color of the LED.
 *
 * @return The 'bsp_result' with the operation result.
 *      @retval  BSP_OK                 If the operation succeeded and the LED has the new color.
 *      @retval  BSP_ERROR_ARG          If the provided argument is invalid.
 *      @retval  BSP_ERROR_UNAVAILABLE  If there is no LED on the board..
 */
bsp_result bsp_led_set_color(bsp_led_color color);

/**
 * @brief  Generate a hard exception that will halt the processor.
 */
void bsp_halt(void);

/**
 * @brief  Suspend the process for time_us microseconds.
 *
 * @param[in]  sleep_time_ms    Time to sleep the process in milliseconds.
 */
void bsp_sleep(unsigned int sleep_time_ms);


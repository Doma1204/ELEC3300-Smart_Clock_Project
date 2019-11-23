#ifndef _WS2812_H
#define _WS2812_H

#include <string.h>
#include "tim.h"
#include "main.h"

#define NUM_LED 64

/* The length of the LED PWM DMA buffer,
 * the number represents how many LED data can transfer in half of the buffer.
 * Actual buffer array length is LED_BUFFER_LENGTH * 2 * 24
 */
#define LED_BUFFER_LENGTH 11

// PWM Output compare value of bit 1 and 0
#define BIT_0 20
#define BIT_1 40

/* Basic function to control a WS2812 LED Strip */
void ws2812_init(void); // init the ws2812
void ws2812_set_brightness(uint8_t b); // set the brightness of LEDs (range: 0-255)
void ws2812_clear(void); // set all LEDs color to (0, 0, 0) and turn off all LEDs
void ws2812_fill(uint32_t first, uint32_t count, uint8_t r, uint8_t g, uint8_t b); // set the color of LEDs from i to i+j
void ws2812_fill_with_buffer(uint8_t* buffer);
void ws2812_set_led_color(uint32_t i, uint8_t r, uint8_t g, uint8_t b); // set the color of a specific LED
void ws2812_update(void); // send out the changes of LEDs
void ws2812_HT(DMA_HandleTypeDef* hdma);

#endif

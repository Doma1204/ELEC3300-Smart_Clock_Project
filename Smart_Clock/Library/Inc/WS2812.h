#ifndef _WS2812_H
#define _WS2812_H

#include <string.h>
#include "main.h"
#include "gpio.h"
#include "i2c.h"
#include "led.h"

#define NUM_WS2812_LED 64
#define PANEL_ROW_WIDTH 8
#define PANEL_COL_WIDTH 8
#define MAX_NUM_PANEL 4

#define PIXEL_LENGTH NUM_WS2812_LED * 3

// For panel detection
#define BIT0_TIME 2
#define BIT1_TIME 3
#define RESET_TIME 4
#define EMIT_TIMES 5

typedef enum _DIRECTION {
    TOP = 0,
    LEFT,
    BOTTOM,
    RIGHT,
    NONE
} Direction;

typedef struct _WS2812_STRUCT {
    uint16_t x;
    uint16_t y;
    uint16_t addr;
    uint8_t modify;
    uint8_t pixel[PIXEL_LENGTH];
} WS2812_STRUCT;

void ws2812_i2c_init(void);
void ws2812_start_search_panel(void);
void ws2812_panel_count_inc(void);

uint16_t get_panel_count(void);

void ws2812_set_brightness(uint8_t b);
uint8_t ws2812_get_brightness(void);

void ws2812_clear(uint16_t X, uint16_t Y);
void ws2812_clear_all(void);
void ws2812_set_pixel(uint16_t X, uint16_t Y, uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void ws2812_fill(uint16_t X, uint16_t Y, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t r, uint8_t g, uint8_t b);
void ws2812_print(uint16_t X, uint16_t Y, uint8_t x, uint8_t y, const char* str , uint8_t r, uint8_t g, uint8_t b);
void ws2812_print_by_id(uint16_t X, uint16_t Y, uint8_t x, uint8_t y, const char* str , uint8_t r, uint8_t g, uint8_t b);

void ws2812_update(void);

#endif

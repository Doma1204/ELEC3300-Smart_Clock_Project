#include "WS2812.h"

static uint16_t panel_count = 0;
static WS2812_struct panel[MAX_NUM_PANEL] = {0};
static uint8_t panel_look_up[4][4] = {0};
static uint8_t brightness = 100;

void ws2812_init(void) {
    // TODO: do the init process
}

void ws2812_start_detect_panel(void) {
    // TODO: do the detect process
}

void ws2812_set_brightness(uint8_t b) {
    if (brightness <= 100) brightness = b;
}

uint8_t ws2812_get_brightness(void) {
    return brightness;
}

void ws2812_clear(uint16_t X, uint16_t Y) {
    uint8_t index = panel_look_up[X][Y];
    memset(&panel[index].pixel, 0, PIXEL_LENGTH);
    panel[index].modify = 1;
}

void ws2812_clear_all(void) {
    for (uint16_t i = 0; i < panel_count; ++i) {
        ws2812_clear(panel[i].x, panel[i].y);
    }∂
}

void ws2812_set_pixel(uint16_t X, uint16_t Y, uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t index = panel_look_up[X][Y];
    uint8_t* pixel = &panel[index].pixel[(y*PANEL_ROW_WIDTH+x)*3];
    pixel[0] = r;
    pixel[1] = g;
    pixel[2] = b;
    panel[index].modify = 1;
}

void ws2812_fill(uint16_t X, uint16_t Y, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t r, uint8_t g, uint8_t b) {
    for (uint16_t x = x1; x <= x2; ++x) {
        for (uint16_t y = y1; y <= y2; ++y) {
            ws2812_set_pixel(X, Y, x, y, r, g, b);
        }
    }
}

void ws2812_update(void) {
    // for (uint8_t i = 0 )
}


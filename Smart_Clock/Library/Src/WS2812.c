#include "WS2812.h"
#include "WS2812_font.h"

volatile uint16_t panel_count = 0;
static WS2812_STRUCT panel[MAX_NUM_PANEL] = {0};
static uint8_t panel_look_up[4][4] = {0};
static uint8_t brightness = 100;

static GPIO
SCL_PORT = {RGB_SCL_GPIO_Port, RGB_SCL_Pin},
SDA_PORT = {RGB_SDA_GPIO_Port, RGB_SDA_Pin},
DIR_PORT = {RGB_GPIO2_GPIO_Port, RGB_GPIO2_Pin};

void ws2812_init(void) {
    // TODO: do the init process
    // for debug only
    panel_count = 1;
    panel_look_up[0][0] = 0;
    panel[0].x = 0;
    panel[0].y = 0;
    panel[0].addr = 0x01;
}

__forceinline static void identify_panel_position(uint8_t* buffer) {
    // int8_t last_x = 0, last_y = 0;
    
    // for (uint8_t i = 0; i < panel_count && i < MAX_NUM_PANEL; ++i) {
    //     Direction dir;
    //     for (uint8_t j = 0; j < 4; ++j) {
    //         if (buffer[i] & (1 << (4+i))) {
    //             dir = (Direction) i;
    //             break;
    //         }
    //     }

        
    // }
}

void ws2812_i2c_init(void) {
    // HAL_GPIO_DeInit(RGB_SCL_GPIO_Port, RGB_SCL_Pin);
    // HAL_GPIO_DeInit(RGB_SDA_GPIO_Port, RGB_SDA_Pin);
    // MX_I2C2_Init();
    // HAL_Delay(50);

    // uint8_t cmd = 0x01;
    // uint8_t buffer[MAX_NUM_PANEL] = {0};
    // for (uint8_t i = 0; i < panel_count && i < MAX_NUM_PANEL; ++i) {
    //     HAL_I2C_Master_Transmit(&hi2c2, i+1, &cmd, 1, 100);
    //     HAL_I2C_Master_Receive(&hi2c2, i+1, &buffer[i], 1, 100);
    // }

    // identify_panel_position(buffer);
}

static void delay(uint32_t time) {
    uint32_t endTime = HAL_GetTick() + time;
    while (endTime - HAL_GetTick() > 0);
}

__forceinline static void setHigh(const GPIO* gpio, uint32_t delay_time) {
    gpio->port->BSRR = gpio->pin;
    delay(1);
    gpio->port->BSRR = (uint32_t)gpio->pin << 16U;
    delay(delay_time - 1);
}

__forceinline static void emit_top_signal(const GPIO* gpio) { // long - short
    setHigh(gpio, BIT1_TIME);
    setHigh(gpio, BIT0_TIME);
    setHigh(gpio, RESET_TIME);
}

__forceinline static void emit_bottom_signal(const GPIO* gpio) { // long - long
    setHigh(gpio, BIT1_TIME);
    setHigh(gpio, BIT1_TIME);
    setHigh(gpio, RESET_TIME);
}

__forceinline static void emit_left_signal(const GPIO* gpio) { // short - short
    setHigh(gpio, BIT0_TIME);
    setHigh(gpio, BIT0_TIME);
    setHigh(gpio, RESET_TIME);
}

__forceinline static void emit_right_signal(const GPIO* gpio) { // short - long
    setHigh(gpio, BIT0_TIME);
    setHigh(gpio, BIT1_TIME);
    setHigh(gpio, RESET_TIME);
}

void ws2812_start_search_panel(void) {
    // TODO: do the detect process
    signal_detect_falling_gpio_init(&SCL_PORT);
    signal_detect_falling_gpio_init(&SDA_PORT);
    signal_emit_gpio_init(&DIR_PORT);

    panel_count = 0;
    for (uint8_t i = 0; i < EMIT_TIMES; ++i) {
        if (panel_count) break;
        emit_left_signal(&DIR_PORT);
    }
}

void ws2812_panel_count_inc(void) {++panel_count;}

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
    }
}

void ws2812_set_pixel(uint16_t X, uint16_t Y, uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    if (x > 8 && y > 8) return;
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

// void ws2812_print(uint16_t X, uint16_t Y, uint8_t x, uint8_t y, char* str , uint8_t r, uint8_t g, uint8_t b) {
//     // uint8_t index = panel_look_up[X][Y];
//     for (uint8* pixel = getChar(*str); *str != '\0'; ++str) {
//         for (uint8_t i = )
//     }
//     for (uint8_t y = y1; y <= y2; ++y) {
//         for (uint8_t x = x1; x <= x2; ++x) {
            
//         }
//     }
// }

void ws2812_update(void) {
    for (uint8_t i = 0; i < panel_count; ++i) {
        if (panel[i].modify || 1) {
            // led_toggle(LED2);
            // while(HAL_I2C_Master_Transmit(&hi2c2, 0x02, panel[i].pixel, PIXEL_LENGTH, 0xFFFF)!= HAL_OK);
            HAL_I2C_Master_Transmit(&hi2c2, 0x02, panel[i].pixel, PIXEL_LENGTH, 0xFFFF);
            panel[i].modify = 0;
        }
    }
}


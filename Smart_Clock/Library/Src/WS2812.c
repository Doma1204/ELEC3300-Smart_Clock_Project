#include "WS2812.h"
#include "WS2812_font.h"

volatile uint16_t panel_count = 0;
WS2812_STRUCT panel[MAX_NUM_PANEL] = {0};
volatile uint8_t panel_look_up[MAX_NUM_PANEL][MAX_NUM_PANEL] = {0};
static uint8_t brightness = 100;
volatile uint8_t rgb_inited = 0;

static GPIO
SCL_PORT = {RGB_SCL_GPIO_Port, RGB_SCL_Pin},
SDA_PORT = {RGB_SDA_GPIO_Port, RGB_SDA_Pin},
DIR_PORT = {RGB_GPIO2_GPIO_Port, RGB_GPIO2_Pin};

uint16_t get_panel_count(void) {return panel_count;}

uint8_t findChild(uint8_t* buffer, uint8_t start, uint8_t mask) {
    for (uint8_t i = start; i < panel_count; ++i) {
        if ((buffer[i] & mask)) return i;
    }
    return 0;
}

void setCoordinate(uint8_t child, uint8_t x, uint8_t y) {
    panel_look_up[x][y] = child;
    panel[child].x = x;
    panel[child].y = y;
    panel[child].addr = (child+1)*2;
}

__forceinline static void identify_panel_position(uint8_t* buffer) {
    panel_look_up[0][0] = 0;
    panel[0].x = 0;
    panel[0].y = 0;
    panel[0].addr = 0x02;

    #define TO_TOP_MASK      0x01
    #define TO_LEFT_MASK     0x02
    #define TO_BOTTOM_MASK   0x04
    #define TO_RIGHT_MASK    0x08

    #define FROM_TOP_MASK    0x10
    #define FROM_LEFT_MASK   0x20
    #define FROM_BOTTOM_MASK 0x40
    #define FROM_RIGHT_MASK  0x80
    
    for (uint8_t i = 0; i < panel_count; ++i) {
        uint16_t last_x = panel[i].x, last_y = panel[i].y;
        if (buffer[i] & TO_TOP_MASK) {
            uint8_t child = findChild(buffer, i+1, FROM_BOTTOM_MASK);
            if (child) setCoordinate(child, last_x, last_y+1);
        } else if (buffer[i] & TO_LEFT_MASK) {
            uint8_t child = findChild(buffer, i+1, FROM_RIGHT_MASK);
            if (child) {setCoordinate(child, last_x+1, last_y); led_toggle(LED2);}
        } else if (buffer[i] & TO_RIGHT_MASK) {
            uint8_t child = findChild(buffer, i+1, FROM_LEFT_MASK);
            if (child) setCoordinate(child, last_x-1, last_y);
        }

        while (++i < panel_count) {
            if (buffer[i] & 0x0F) break;
        }
    }
}

void ws2812_i2c_init(void) {
    HAL_GPIO_DeInit(DIR_PORT.port, DIR_PORT.pin);
    HAL_GPIO_DeInit(RGB_SCL_GPIO_Port, RGB_SCL_Pin);
    HAL_GPIO_DeInit(RGB_SDA_GPIO_Port, RGB_SDA_Pin);
    rgb_inited = 1;
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

void signal_detect_falling_gpio_init(GPIO* gpio) {
    HAL_GPIO_DeInit(gpio->port, gpio->pin);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = gpio->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(gpio->port, &GPIO_InitStruct);
}

void signal_emit_gpio_init(GPIO* gpio) {
    HAL_GPIO_DeInit(gpio->port, gpio->pin);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = gpio->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(gpio->port, &GPIO_InitStruct);
}

void ws2812_start_search_panel(void) {
    // TODO: do the detect process
    signal_detect_falling_gpio_init(&SCL_PORT);
    signal_emit_gpio_init(&DIR_PORT);

    panel_count = 0;
    for (uint8_t i = 0; i < EMIT_TIMES; ++i) {
        if (panel_count) break;
        emit_left_signal(&DIR_PORT);
    }
    signal_detect_falling_gpio_init(&DIR_PORT);
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

void ws2812_set_pixel_by_id(uint16_t id, uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    if (x > 8 && y > 8) return;
    uint8_t* pixel = &panel[id].pixel[(y*PANEL_ROW_WIDTH+x)*3];
    pixel[0] = r;
    pixel[1] = g;
    pixel[2] = b;
    panel[id].modify = 1;
}

void ws2812_fill(uint16_t X, uint16_t Y, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t r, uint8_t g, uint8_t b) {
    for (uint16_t x = x1; x <= x2; ++x) {
        for (uint16_t y = y1; y <= y2; ++y) {
            ws2812_set_pixel(X, Y, x, y, r, g, b);
        }
    }
}

void ws2812_print(uint16_t X, uint16_t Y, uint8_t x, uint8_t y, const char* str , uint8_t r, uint8_t g, uint8_t b) {
    for (; *str != '\0'; ++str) {
        uint8_t* pixel = getChar(*str);
        for (uint8_t i = 1; i < *pixel + 1; ++i, ++x) {
            if (x >= 8) {
                if ((X-1 == 0 && Y == 0) || panel_look_up[X-1][Y] != 0) {
                    X -= 1;
                    x = 0;
                } else {
                    return;
                }
            }
            for (uint8_t yy = 0; yy < 8 - y; ++yy) {
                if (pixel[i] & (1 << yy)) ws2812_set_pixel(X, Y, x, yy, r, g, b);
                else ws2812_set_pixel(X, Y, x, yy, 0, 0, 0);
            }
        }
        ++x;
    }
}

void ws2812_print_by_id(uint16_t X, uint16_t Y, uint8_t x, uint8_t y, const char* str , uint8_t r, uint8_t g, uint8_t b) {
    for (; *str != '\0'; ++str) {
        uint8_t* pixel = getChar(*str);
        for (uint8_t i = 1; i < *pixel + 1; ++i, ++x) {
            for (uint8_t yy = 0; yy < 8 - y; ++yy) {
                if (pixel[i] & (1 << yy)) ws2812_set_pixel_by_id(X, x, yy, r, g, b);
                else ws2812_set_pixel_by_id(X, x, yy, 0, 0, 0);
            }
        }
        ++x;
    }
}

uint8_t wsBuffer[MAX_NUM_PANEL] = {0};
void ws2812_update(void) {
    if (rgb_inited == 1) {
        MX_I2C2_Init();
        HAL_Delay(500);

        uint8_t cmd = 0x01;
        for (uint8_t i = 1; i <= panel_count && i < MAX_NUM_PANEL; ++i) {
            HAL_I2C_Master_Transmit(&hi2c2, i*2, &cmd, 1, 1000);
            HAL_I2C_Master_Receive(&hi2c2, i*2, &wsBuffer[i-1], 1, 100);
        }

        panel_count = 0;
        for (uint8_t i = 0; i < MAX_NUM_PANEL; ++i) {
            if (wsBuffer[i]) ++panel_count;
        }

        identify_panel_position(wsBuffer);
        rgb_inited = 2;
    } else if (rgb_inited == 2) {
        for (uint8_t i = 0; i < panel_count; ++i) {
            if (panel[i].modify) {
                if (HAL_I2C_Master_Transmit(&hi2c2, (i+1)*2, panel[i].pixel, PIXEL_LENGTH, 100) == HAL_OK) {
                    panel[i].modify = 0;
                }
            }
        }
    }
}


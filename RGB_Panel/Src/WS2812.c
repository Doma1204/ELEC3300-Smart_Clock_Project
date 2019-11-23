#include "WS2812.h"

static uint8_t led_color[NUM_LED*3];
static uint8_t brightness = 10;
static uint8_t transfer_state = 0;

/* Basic function to control a WS2812 LED Strip */
void ws2812_init(void) {
    ws2812_clear();
}

void ws2812_set_brightness(uint8_t b) {brightness = b;}

void ws2812_clear(void) {
    memset(led_color, 0, sizeof(led_color));
    // ws2812_fill(0, 64, 50, 50, 50);
    ws2812_update();
}

void ws2812_fill(uint32_t first, uint32_t count, uint8_t r, uint8_t g, uint8_t b) {
    for (uint32_t i = first; i < first + count; ++i) {
        if (i >= NUM_LED) break;
        ws2812_set_led_color(i, r, g, b);
    }
}

void ws2812_fill_with_buffer(uint8_t* buffer) {
    memcpy(&led_color, buffer, 64*3);
}

void ws2812_set_led_color(uint32_t i, uint8_t r, uint8_t g, uint8_t b) {
    if (i < NUM_LED && brightness) {
        uint8_t* led = &led_color[i*3];
        led[0] = ((r + 1) * brightness) >> 8;
        led[1] = ((g + 1) * brightness) >> 8;
        led[2] = ((b + 1) * brightness) >> 8;
    }
}

static uint8_t buffer[LED_BUFFER_LENGTH * 2 * 24];
static uint32_t buffer_index, led_index;

void put_buffer(void) {
    uint8_t* buff = &buffer[buffer_index*24];
    uint8_t* led = &led_color[led_index*3];
    for (uint8_t i = 0; i < 8; ++i) {
        buff[i]    = led[1] & (1 << (7-i)) ? BIT_1 : BIT_0;
        buff[8+i]  = led[0] & (1 << (7-i)) ? BIT_1 : BIT_0;
        buff[16+i] = led[2] & (1 << (7-i)) ? BIT_1 : BIT_0;
    }

    ++buffer_index;
    ++led_index;
}

void fill_buffer(uint8_t isFull) { // isFull: 0: fill first half of the buffer, 1: fill all buffer or second half of the buffer depends on situation
    uint32_t buffer_length = isFull ? LED_BUFFER_LENGTH * 2 : LED_BUFFER_LENGTH;

    while (buffer_index < buffer_length && led_index < NUM_LED) put_buffer();
    if (buffer_index < buffer_length) memset(&buffer[buffer_index*24], 0, (buffer_length - buffer_index) * 24);

    buffer_index = isFull ? 0 : LED_BUFFER_LENGTH;
}

void ws2812_update(void) {
    if (transfer_state) return;
    transfer_state = 1;

    memset(buffer, 0, 48); // to make a reset signal
    buffer_index = 2;
    led_index = 0;
    fill_buffer(1);

    // HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*) buffer, LED_BUFFER_LENGTH * 2 * 24);
}

void ws2812_HT(DMA_HandleTypeDef* hdma) {
    if (hdma == &hdma_tim3_ch1_trig) {
        if (transfer_state == 2) return;
        fill_buffer(0);
    }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
    if (htim == &htim3) {
        if (transfer_state == 2) {
            HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
            // HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
            transfer_state = 0;
        } else {
            fill_buffer(1);
            if (led_index == NUM_LED) {
                transfer_state = 2;
            }
		}
    }
}

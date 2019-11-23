#include "led.h"

static LED_STRUCT LEDS[NUM_LED] = {
    {{LED1_GPIO_Port, LED1_Pin}, 0},
    {{LED2_GPIO_Port, LED2_Pin}, 0},
    {{LED3_GPIO_Port, LED3_Pin}, 0}
};

void led_on(LED led) {
    HAL_GPIO_WritePin(LEDS[led].gpio.port, LEDS[led].gpio.pin, GPIO_PIN_SET);
}

void led_off(LED led) {
    HAL_GPIO_WritePin(LEDS[led].gpio.port, LEDS[led].gpio.pin, GPIO_PIN_RESET);
}

void led_toggle(LED led) {
    HAL_GPIO_TogglePin(LEDS[led].gpio.port, LEDS[led].gpio.pin);
}

void led_blink(LED led, uint32_t frequency) {
    LEDS[led].frequency = frequency;
}

void led_update(void) {
    static uint32_t last_led_ticks = 0;
    uint32_t this_ticks = HAL_GetTick();
    if (this_ticks - last_led_ticks >= LED_UPDATE_FREQUENCY) {
        for (uint8_t i = 0; i < NUM_LED; ++i) {
            if (LEDS[i].frequency && this_ticks - LEDS[i].last_ticks >= LEDS[i].frequency) {
                led_toggle((LED) i);
                LEDS[i].last_ticks = this_ticks;
            }
        }
        last_led_ticks = HAL_GetTick();
    }
}


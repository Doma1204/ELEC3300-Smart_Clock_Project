#ifndef _LED_H
#define _LED_H

#include "main.h"
#include "gpio.h"

#define NUM_LED 3
#define LED_UPDATE_FREQUENCY 50

typedef enum _LED {
    LED1 = 0,
    LED2,
    LED3
} LED;

typedef struct _LED_STRUCT {
    GPIO gpio;
    uint32_t frequency;
    uint32_t last_ticks;
} LED_STRUCT;

void led_on(LED led);
void led_off(LED led);
void led_toggle(LED led);
void led_blink(LED led, uint32_t frequency);
void led_update(void);

#endif

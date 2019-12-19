#ifndef _BUTTON_H
#define _BUTTON_H

#include "main.h"
#include "gpio.h"

#define NUM_BUTTON 3
#define BUTTON_UPDATE_FREQUENCY 50

typedef void (*ButtonListener)(void);

typedef enum _BUTTON {
    BUTTON1 = 0,
    BUTTON2,
    BUTTON3
} BUTTON;

typedef struct _BUTTON_STRUCT {
    GPIO gpio;
    uint32_t pressed;
    uint32_t threshold;
    uint32_t frequency;
    uint8_t repeated;
    ButtonListener onClickListener;
    ButtonListener onHoldListener;
    ButtonListener onReleaseListener;
} BUTTON_STRUCT;

uint8_t is_button_pressed(BUTTON btn);
void button_update(void);

void set_button_onClickListener(BUTTON btn, ButtonListener listener);
void set_button_onHoldListener(BUTTON btn, uint32_t threshold, uint32_t frequency, ButtonListener listener);
void set_button_onReleaseListener(BUTTON btn, ButtonListener listener);

void drop_button_onClickListener(BUTTON btn);
void drop_button_onHoldListener(BUTTON btn);
void drop_button_onReleaseListener(BUTTON btn);

void drop_button_all_listener(BUTTON btn);
void drop_all_button_listener(void);

#endif

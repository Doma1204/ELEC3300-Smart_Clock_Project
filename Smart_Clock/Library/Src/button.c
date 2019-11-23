#include "button.h"

static BUTTON_STRUCT BUTTONS[NUM_BUTTON] = {
    {{BTN1_GPIO_Port, BTN1_Pin}, 0, 0, 0, 0, 0, 0, 0},
    {{BTN2_GPIO_Port, BTN2_Pin}, 0, 0, 0, 0, 0, 0, 0},
    {{BTN3_GPIO_Port, BTN3_Pin}, 0, 0, 0, 0, 0, 0, 0}
};

uint8_t is_button_pressed(BUTTON btn) {
    return !HAL_GPIO_ReadPin(BUTTONS[btn].gpio.port, BUTTONS[btn].gpio.pin);
}

void button_update(void) {
    static uint32_t last_button_ticks = 0;
    uint32_t this_ticks = HAL_GetTick();
    if (this_ticks - last_button_ticks < BUTTON_UPDATE_FREQUENCY) return;

    for (uint8_t i = 0; i < NUM_BUTTON; ++i) {
        if (is_button_pressed((BUTTON) i)) {
            if (!BUTTONS[i].pressed && BUTTONS[i].onClickListener) {
                BUTTONS[i].onClickListener();
            } else if (BUTTONS[i].onHoldListener
                && BUTTONS[i].pressed > BUTTONS[i].threshold
                && ((BUTTONS[i].frequency || !BUTTONS[i].repeated)
                || !((BUTTONS[i].pressed - BUTTONS[i].threshold) % BUTTONS[i].frequency))) {
                BUTTONS[i].onHoldListener();
                BUTTONS[i].repeated = 1;
            }
            ++BUTTONS[i].pressed;
        } else {
            if (BUTTONS[i].pressed && BUTTONS[i].onReleaseListener) {
                BUTTONS[i].onReleaseListener();
            }
            BUTTONS[i].pressed = 0;
            BUTTONS[i].repeated = 0;
        }
    }

    last_button_ticks = this_ticks;
}

void set_button_onClickListener(BUTTON btn, ButtonListener listener) {
    BUTTONS[btn].onClickListener = listener;
}

void set_button_onHoldListener(BUTTON btn, uint32_t threshold, uint32_t frequency, ButtonListener listener) {
    BUTTONS[btn].onHoldListener = listener;
    BUTTONS[btn].threshold = threshold;
    BUTTONS[btn].frequency = frequency;
}

void set_button_onReleaseListener(BUTTON btn, ButtonListener listener) {
    BUTTONS[btn].onReleaseListener = listener;
}

void drop_button_onClickListener(BUTTON btn) {
    BUTTONS[btn].onClickListener = 0;
}

void drop_button_onHoldListeren(BUTTON btn) {
    BUTTONS[btn].onHoldListener = 0;
}

void drop_button_onReleaseListeren(BUTTON btn) {
    BUTTONS[btn].onReleaseListener = 0;
}


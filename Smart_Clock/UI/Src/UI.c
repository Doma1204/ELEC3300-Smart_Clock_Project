#include "UI.h"

TFT_MODE tft_mode = MAIN_SCREEN;

void UI_init(void) {
    set_main_tft_screen();
}

void set_tft_mode(TFT_MODE mode) {tft_mode = mode;}

void UI_update(void) {
    tft_clear();
    switch (tft_mode) {
        case MAIN_SCREEN: main_tft_screen(); break;
    }

    // tft_main_update();
    static uint8_t past = 0;
    uint8_t this_pir = getPIR();
    if (past != this_pir) {
        if (this_pir) {
            tft_set_brightness(999);
        } else {
            tft_set_brightness(0);
            ws2812_clear_all();
        }
    }
    if (getPIR()) {
		tft_set_brightness(999);
	} else {
		tft_set_brightness(0);
	}
}

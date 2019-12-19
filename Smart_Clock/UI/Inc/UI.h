#ifndef _UI_H
#define _UI_H

#include "main_tft_screen.h"
#include "WS2812.h"

typedef enum _TFT_MODE {
    MAIN_SCREEN = 0
} TFT_MODE;

void UI_init(void);
void UI_update(void);
void set_tft_mode(TFT_MODE mode);

#endif

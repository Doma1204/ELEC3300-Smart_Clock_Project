#ifndef _WEATHER_WIDGET_H
#define _WEATHER_WIDGET_H

#include "tft.h"
#include "UI.h"
#include "file.h"
#include "strhelper.h"
#include "esp8266.h"

void tft_print_forecast_widget(uint16_t x, uint16_t y, uint16_t text_x, uint8_t month, uint8_t day, uint8_t min_temp, uint8_t max_temp, uint8_t icon);
void tft_print_weather_widget(uint8_t location, uint8_t temp, uint8_t icon);

#endif

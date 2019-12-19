#include "main_tft_screen.h"

extern uint8_t cur_location;
extern uint8_t cur_temp;
extern uint8_t cur_icon;
extern uint8_t fore_icon[FORECAST_DAY], fore_max[FORECAST_DAY], fore_min[FORECAST_DAY];


void up(void) {
    if (cur_location == 0) return;
    requestTemperature(cur_location - 1);
}

void down(void) {
    if (cur_location == 26) return;
    requestTemperature(cur_location + 1);
}

void set_main_tft_screen(void) {
    drop_all_button_listener();
    set_button_onClickListener(BUTTON1, up);
    set_button_onClickListener(BUTTON2, down);

    set_tft_mode(MAIN_SCREEN);
    tft_print_forecast_widget(30, 130, 3, month(), day() + 1, fore_min[0], fore_max[0], fore_icon[0]);
    tft_print_forecast_widget(140, 130, 17, month(), day() + 2, fore_min[1], fore_max[1], fore_icon[1]);
    tft_print_forecast_widget(250, 130, 31, month(), day() + 3, fore_min[2], fore_max[2], fore_icon[2]);
    tft_print_weather_widget(cur_location, cur_temp, cur_icon);
}

void main_tft_screen(void) {
    #include "PIR.h"
    tft_prints(0, 2, "PIR%d", getPIR());
    convertTime(now(), 0);
    // tft_prints(0, 0, "%d", gy39_getTemperature());
    tft_prints(0, 1, "Temperature: %d.%dC", gy39_getTemperature() / 100, gy39_getTemperature() % 100);
    tft_prints(21, 0, "%04d/%02d/%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
    tft_print_forecast_widget(30, 130, 3, month(), day() + 1, fore_min[0], fore_max[0], 0);
    tft_print_forecast_widget(140, 130, 17, month(), day() + 2, fore_min[1], fore_max[1], 0);
    tft_print_forecast_widget(250, 130, 31, month(), day() + 3, fore_min[2], fore_max[2], 0);
    tft_print_weather_widget(cur_location, cur_temp, cur_icon);
}

#include "weather_widget.h"

uint8_t icon_buff[350] = {0};

UINT numOfBytes = 350;

void tft_print_forecast_widget(uint16_t x, uint16_t y, uint16_t text_x, uint8_t month, uint8_t day, uint8_t min_temp, uint8_t max_temp, uint8_t icon) {
    if (icon) {
        char icon_str[3];
        utos(icon_str, icon);
        FRESULT res = read_file_to_buffer(icon_str, icon_buff, &numOfBytes);
        if (res != FR_OK) tft_print_mono(icon_buff, x, y, 50, 50, WHITE, BLACK);
    }

    tft_prints(text_x, 12, "%d-%dC", min_temp, max_temp);
    tft_prints(text_x + 1, 13, "%d/%d", day, month);
}

void tft_print_weather_widget(uint8_t location, uint8_t temp, uint8_t icon) {
    if (icon) {
        char icon_str[3];
        utos(icon_str, icon);
        FRESULT res = read_file_to_buffer(icon_str, icon_buff, &numOfBytes);
        if (res != FR_OK) tft_print_mono(icon_buff, 60, 50, 50, 50, WHITE, BLACK);
    }

    tft_prints(15, 3, Location_str[location]);
    tft_prints(15, 4, "%dC", temp);
}

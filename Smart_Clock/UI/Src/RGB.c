#include "RGB.h"
#include "strhelper.h"

extern volatile uint16_t panel_count;

void printCalendar(uint16_t X, uint16_t Y) {
    static uint8_t last_day = 0;
    uint8_t this_day = day();
    if (last_day != this_day) {
        last_day = this_day;
        ws2812_fill(X, Y, 0 ,0, 8, 2, 10, 0, 0);
        char d_str[3];
        if (this_day < 10) {
            d_str[0] = '0';
            utos0(&d_str[1], this_day, 1);
        } else {
            utos0(d_str, this_day, 2);
        }
        ws2812_print(X, Y, 1, 2, d_str, 10, 10, 10);
    }
}

void rgb_update(void) {
    switch (panel_count) {
        case 2: {
            static uint8_t oldhour = 0;
            if (oldhour != hour()) {
                char temphour[3];
                if (hour() < 10) {
                temphour[0] = '0';
                utos0(&temphour[1], hour(), 1);
                } else {
                utos0(temphour, hour(), 2);
                }
                ws2812_print(1, 0, 0, 0, temphour, 10, 10, 10);
                oldhour = hour();
            }

            static uint8_t oldminute = 0;
            if (oldminute != minute()) {
                char tempminute[3];
                if (minute() < 10) {
                tempminute[0] = '0';
                utos0(&tempminute[1], minute(), 1);
                } else {
                utos0(tempminute, minute(), 2);
                }
                ws2812_print(0, 0, 0, 0, tempminute, 10, 10, 10);
                oldminute = minute();
            }
            break;
        }

        case 4:
            printCalendar(3, 0);

        case 3: {
            static uint8_t oldhour = 0;
            if (oldhour != hour()) {
                char temphour[3];
                if (hour() < 10) {
                temphour[0] = '0';
                utos0(&temphour[1], hour(), 1);
                } else {
                utos0(temphour, hour(), 2);
                }
                ws2812_print(2, 0, 0, 0, temphour, 10, 10, 10);
                oldhour = hour();
            }

            static uint8_t oldminute = 0;
            if (oldminute != minute()) {
                char tempminute[3];
                if (minute() < 10) {
                tempminute[0] = '0';
                utos0(&tempminute[1], minute(), 1);
                } else {
                utos0(tempminute, minute(), 2);
                }
                ws2812_print(1, 0, 0, 0, tempminute, 10, 10, 10);
                oldminute = minute();
            }

            static uint8_t oldSecond = 0;
            if (oldSecond != second()) {
                char tempSec[3];
                if (second() < 10) {
                tempSec[0] = '0';
                utos0(&tempSec[1], second(), 1);
                } else {
                utos0(tempSec, second(), 2);
                }
                ws2812_print(0, 0, 0, 0, tempSec, 10, 10, 10);
                oldSecond = second();
            }
            break;
        }
    }
}


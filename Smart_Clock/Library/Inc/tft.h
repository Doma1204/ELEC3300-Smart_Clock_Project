#ifndef __TFT_H
#define __TFT_H

#include "gpio.h"
#include "spi.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

typedef enum {
	PIN_ON_TOP,
	PIN_ON_LEFT,
	PIN_ON_BOTTOM,
	PIN_ON_RIGHT
}TFT_ORIENTATION;

/* GPIO settings */

/* Colors */
#define	RGB888TO565(RGB888)  (((RGB888 >> 8) & 0xF800) |((RGB888 >> 5) & 0x07E0) | ((RGB888 >> 3) & 0x001F))

#define WHITE           (RGB888TO565(0xFFFFFF))
#define BLACK           (RGB888TO565(0x000000))
#define DARK_GREY       (RGB888TO565(0x555555))
#define GREY            (RGB888TO565(0xAAAAAA))
#define RED             (RGB888TO565(0xFF0000))
#define DARK_RED        (RGB888TO565(0x800000))
#define ORANGE          (RGB888TO565(0xFF9900))
#define YELLOW          (RGB888TO565(0xFFFF00))
#define GREEN           (RGB888TO565(0x00FF00))
#define DARK_GREEN      (RGB888TO565(0x00CC00))
#define BLUE            (RGB888TO565(0x0000FF))
#define BLUE2           (RGB888TO565(0x202060))
#define SKY_BLUE        (RGB888TO565(0x11CFFF))
#define CYAN            (RGB888TO565(0x8888FF))
#define PURPLE          (RGB888TO565(0x00AAAA))
#define PINK            (RGB888TO565(0xC71585))
#define GRAYSCALE(S)         (2113*S)

#define TFT_SPI hspi1

/* Pixels */
#define CHAR_WIDTH              8
#define CHAR_HEIGHT             16

#define SCREEN_CHOSEN
#define MAX_WIDTH               240
#define MAX_HEIGHT              320

// #define CHAR_BUF_SIZE           600

#define CHAR_MAX_X_VERTICAL     30
#define CHAR_MAX_Y_VERTICAL     20

#define CHAR_MAX_X_HORIZONTAL   40
#define CHAR_MAX_Y_HORIZONTAL   15

#define CHAR_MAX_X              40      // max between CHAR_MAX_X_VERTICAL and CHAR_MAX_X_HORIZONTAL
#define CHAR_MAX_Y              20      // max between CHAR_MAX_Y_VERTICAL and CHAR_MAX_Y_HORIZONTAL

#define TFT_ORIENTATION_CMD    0x36
#define TFT_EOF_MARKER         0x42
#define TFT_EOL_MARKER         0x43
#define TFT_DELAY_MARKER       0xFF

// extern uint8_t tft_orientation;
// extern uint16_t curr_bg_color;
// extern uint16_t curr_text_color;
// extern uint16_t curr_text_color_sp;
// extern uint16_t curr_highlight_color;

// extern char text_buf[2][CHAR_MAX_X][CHAR_MAX_Y];
// extern uint16_t text_color_buf[2][CHAR_MAX_X][CHAR_MAX_Y];
// extern uint16_t bg_color_buf[2][CHAR_MAX_X][CHAR_MAX_Y];

/* TFT communication control */
void tft_spi_init(void);
void tft_init(TFT_ORIENTATION orientation, uint16_t in_bg_color, uint16_t in_text_color, uint16_t in_text_color_sp, uint16_t in_highlight_color); //, DISPLAY_MODE mode);

/* Display properties configuration */
void tft_set_bg_color(uint16_t in_bg_color);
void tft_set_text_color(uint16_t in_text_color);
void tft_set_highlight_color(uint16_t sp_bg_color);
void tft_set_special_color(uint16_t text_color_sp);
uint8_t tft_get_orientation(void);

/* Pixel printing */
void tft_put_pixel(uint16_t x, uint16_t y, uint16_t color);
void tft_fill_color(uint16_t color);
void tft_force_clear(void);

/* Text buffer printing */
void tft_clear_line(uint8_t line);
void tft_clear(void);
char tft_get_buffer_char(uint8_t x, uint8_t y);
uint16_t tft_get_buffer_color(uint8_t x, uint8_t y);
inline void tft_printc(uint8_t x, uint8_t y, char * fp);
void tft_prints(uint8_t x, uint8_t y, const char * pstr, ...);
void tft_printi(uint8_t x, uint8_t y, int32_t num);
void tft_printl(uint8_t x, uint8_t y, int64_t num);
void tft_printu(uint8_t x, uint8_t y, uint32_t num);
void tft_printf(uint8_t x, uint8_t y, float num, uint32_t dp);
void tft_update(void);

//If you have space for it you can buffer all pixels in the entire display and push it using DMA
void tft_pxbuf_update(uint16_t* tft_px_buffer); //Update pixel buffer
void tft_pxbuf_write(uint16_t* tft_px_buffer); //Always write text to px buffer
void tft_push_pxbuf(void* buf, uint32_t x, uint32_t y, uint32_t w, uint32_t h); //Push the pixel buffer to the display with DMA
void tft_circ_push_pxbuf(void* buf, uint32_t x, uint32_t y, uint32_t w, uint32_t h); //Set DMA in circular mode, call once

void tft_put_logo(uint16_t x, uint16_t y);

#endif		/* __TFT_H */

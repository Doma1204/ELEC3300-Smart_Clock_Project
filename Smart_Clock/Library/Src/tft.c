#include "tft.h"
#include "tft_font.h"
#include "dma.h"
#include "strhelper.h"

/* tft init */
static const uint8_t tft_init_code[] = { 0x00,

    0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,                     TFT_EOL_MARKER,
    0xCF, 0x00, 0XC1, 0X30,                                 TFT_EOL_MARKER,
    0xE8, 0x85, 0x00, 0x78,                                 TFT_EOL_MARKER,
    0xEA, 0x00, 0x00,                                       TFT_EOL_MARKER,
    0xEA, 0x00, 0x00,                                       TFT_EOL_MARKER,
    0xED, 0x64, 0x03, 0x12, 0x81,                           TFT_EOL_MARKER,
    0xF7, 0x20,                                             TFT_EOL_MARKER,
    // Power control VRH[5:0] = 4.60 V
    0xC0, 0x23,                                             TFT_EOL_MARKER,
    // Power control SAP[2:0];BT[3:0] = adjust power to lowest possible
    0xC1, 0x10,                                             TFT_EOL_MARKER,
    // VCM control 1 Contrast VCOMH = 4.250 VCOML = -1.500
    0xC5, 0x3e, 0x28,                                       TFT_EOL_MARKER,
    // VCM control 2
    0xC7, 0x86,                                             TFT_EOL_MARKER,
    // Memory Access Control - 0x08 = BGR color filter panel, 0x40 = Column Address Order 1
    TFT_ORIENTATION_CMD,                                    TFT_EOL_MARKER,
    // COLMOD: Pixel Format Set 16bits for pixel
    0x3A, 0x55,                                             TFT_EOL_MARKER,
    // Frame Rate Control (In Normal Mode/Full Colors) - division ratio: 1 - 24 clocks
    0xB1, 0x00, 0x18,                                       TFT_EOL_MARKER,
    // Display Function Control
    0xB6, 0x08, 0x82, 0x27,                                 TFT_EOL_MARKER,
    // Exit sleep
    0x11, TFT_DELAY_MARKER, 120,                            TFT_EOL_MARKER,
    0x29,                                                   TFT_EOL_MARKER,
    0x2c,                                                   TFT_EOL_MARKER,

    // EOF - not sent to controller
    TFT_EOF_MARKER
};
static const uint8_t tft_orientation_init_code[] = {
    0x48,
    0x28,
    0x88,
    0xE8,
};

//private data
uint16_t curr_bg_color = BLACK;
uint16_t curr_highlight_color = BLACK;
uint16_t curr_text_color = BLACK;
uint16_t curr_text_color_sp = BLACK;

uint8_t tft_orientation = 0, tft_enabled = 1;
uint8_t char_max_x, char_max_y;
uint16_t row_width;

uint8_t cur_screen, next_screen;
uint8_t pointer_to_curr_screen = 0;
char text_buf[2][CHAR_MAX_X][CHAR_MAX_Y] = {0};
uint16_t text_color_buf[2][CHAR_MAX_X][CHAR_MAX_Y] = {0};
uint16_t bg_color_buf[2][CHAR_MAX_X][CHAR_MAX_Y] = {0};

#define CS_HIGH() TFT_CS1_GPIO_Port->BSRR = TFT_CS1_Pin
#define CS_LOW()  TFT_CS1_GPIO_Port->BSRR = (uint32_t) TFT_CS1_Pin << 16U

#define DC_HIGH() TFT_DC_GPIO_Port->BSRR = TFT_DC_Pin
#define DC_LOW()  TFT_DC_GPIO_Port->BSRR = (uint32_t) TFT_DC_Pin << 16U

static inline void tft_write_command(uint8_t command) {
	DC_LOW();
	CS_LOW();
	HAL_SPI_Transmit(&TFT_SPI, (uint8_t*)&command, 1, 1);
	CS_HIGH();
}
static inline void tft_write_data(uint8_t* data, uint16_t size) {
	DC_HIGH();
	CS_LOW();
	HAL_SPI_Transmit(&TFT_SPI, (uint8_t*)data, size, 1);
	CS_HIGH();
}
static inline void tft_write_many(uint8_t command, uint8_t* data, uint16_t size) {
	tft_write_command(command);
	tft_write_data(data, size);
}
static inline void tft_write_many_dma(uint8_t command, uint8_t* data, uint16_t size) {
	tft_write_command(command);
	HAL_SPI_Transmit_DMA(&TFT_SPI, (uint8_t*)data, size);
}
static inline void tft_write_byte(uint8_t command, uint8_t data) {
	tft_write_command(command);
	tft_write_data(&data, 1);
}
static inline void tft_write_hword(uint8_t command, uint16_t data) {
	tft_write_command(command);
	uint8_t buf[2] = {data>>8, data&0xFF};
	tft_write_data(buf, 2);
}
static inline void tft_write_2hword(uint8_t command, uint16_t a, uint16_t b) {
	tft_write_command(command);
	uint8_t buf[4] = {a>>8, a&0xFF, b>>8, b&0xFF};
	tft_write_data(buf, 4);
}

/**
  * @brief  Hardware reset for TFT
  * @param  None
  * @retval None
  */
void tft_reset(void)
{
	HAL_GPIO_WritePin(TFT_RES_GPIO_Port, TFT_RES_Pin, GPIO_PIN_SET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(TFT_RES_GPIO_Port, TFT_RES_Pin, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(TFT_RES_GPIO_Port, TFT_RES_Pin, GPIO_PIN_SET);
	HAL_Delay(50);
}

/**
  * @brief  Configuration of TFT
  * @param  None
  * @retval None
  */
inline static void tft_config(void)
{
	tft_reset();

	uint32_t i=0;
	while (tft_init_code[++i] != TFT_EOF_MARKER) {
		if (tft_init_code[i] == TFT_ORIENTATION_CMD) {
			tft_write_byte(tft_init_code[i], tft_orientation_init_code[tft_orientation]);
			i++;
		} else if (tft_init_code[i] == TFT_DELAY_MARKER) {
			i += 3;
			HAL_Delay(tft_init_code[i]);
		} else if (tft_init_code[i+1] == TFT_EOL_MARKER) {
			tft_write_command(tft_init_code[i]);
			i++;
		} else {
			int len;
			for (len=0; tft_init_code[i+len+1] != TFT_EOL_MARKER; len++) {}
			tft_write_many(tft_init_code[i], (uint8_t*)&tft_init_code[i+1], len);
			i += len+1;
		}
	}
}

/**
  * @brief  Initialization of TFT
  * @param  orientation: default orientation
  * @param  in_bg_color: default background color
  * @param  in_text_color: default text color
  * @param  in_text_color_sp: default special text color
  * @retval None
  */
void tft_init(TFT_ORIENTATION orientation, uint16_t in_bg_color, uint16_t in_text_color, uint16_t in_text_color_sp, uint16_t in_highlight_color) { //, DISPLAY_MODE mode) {
	cur_screen = 0;
	next_screen = 1;
	tft_orientation = orientation;

	tft_config();
	
	tft_set_bg_color(in_bg_color);
	tft_set_text_color(in_text_color);
	tft_set_special_color(in_text_color_sp);
	tft_set_highlight_color(in_highlight_color);
	
	tft_fill_color(in_bg_color);
	
	char_max_x = (orientation % 2) ? CHAR_MAX_X_HORIZONTAL : CHAR_MAX_X_VERTICAL;
	char_max_y = (orientation % 2) ? CHAR_MAX_Y_HORIZONTAL : CHAR_MAX_Y_VERTICAL;

	row_width = (orientation%2) ? MAX_HEIGHT : MAX_WIDTH;
	
	tft_clear();
	cur_screen = 1;
	tft_clear();
	cur_screen = 0;
	tft_update();
}

/**
  * @brief  Set the current background color
  * @param  None
  * @retval None
  */
void tft_set_bg_color(uint16_t in_bg_color) {
	curr_bg_color = in_bg_color;
}

/**
  * @brief  Set the current special background color
  * @param  None
  * @retval None
  */
void tft_set_highlight_color(uint16_t highlight_color) {
	curr_highlight_color = highlight_color;
}

/**
  * @brief  Set the current text color
  * @param  None
  * @retval None
  */
void tft_set_text_color(uint16_t in_text_color) {
	curr_text_color = in_text_color;
}

/**
  * @brief  Set the current special text color
  * @param  None	
  * @retval None
  */
void tft_set_special_color(uint16_t text_color_sp) {
	curr_text_color_sp = text_color_sp;
}

uint8_t tft_get_orientation() {
	return tft_orientation;
}

static inline void tft_set_region(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	uint16_t startx = x;
	uint16_t starty = y;
	uint16_t endx = x+w;
	uint16_t endy = y+h;

	tft_write_2hword(0x2a, startx, endx);		// Column addr set
	tft_write_2hword(0x2b, starty, endy);		// Column addr set
	// tft_write_command(0x2c);	
}

void tft_fill_color(uint16_t color) {
	if (tft_orientation % 2)
		tft_set_region(0, 0, MAX_HEIGHT, MAX_WIDTH);
	else
		tft_set_region(0, 0, MAX_WIDTH, MAX_HEIGHT);
	
	tft_write_command(0x2c);
	for (uint32_t i = 0; i < MAX_WIDTH*MAX_HEIGHT; i++) {
		uint8_t rgb565[2] = {color>>8, color&0xFF};
		tft_write_data(rgb565, 2);
	}
}

/**
  * @brief  Clear one line in the text buffer
  * @param  line: the line to be cleared
  * @retval None
  */
void tft_clear_line(uint8_t y){
	for (uint8_t x = 0; x < char_max_x; x++) {
		text_buf[cur_screen][x][y] = ' ';
		text_color_buf[cur_screen][x][y] = curr_text_color;
		bg_color_buf[cur_screen][x][y] = curr_bg_color;
	}
}

/**
  * @brief  Clear the whole text buffer
  * @param  None
  * @retval None
  */
void tft_clear(void){
	for(uint8_t y = 0; y < char_max_y; y++){
		tft_clear_line(y);
	}
}

void tft_printc(uint8_t x, uint8_t y, char * fp) {
	uint8_t i = x;
	uint8_t j = y;
	uint8_t is_special = 0;
	uint8_t is_highlighted = 0;
	uint8_t is_underlined = 0;
	// uint8_t escaped = 0;
	
	while (*fp && i < char_max_x && j < char_max_y) {
		switch(*fp) {
			case '\r':
			case '\n':
			j++;
			break;
			case '[':
			case ']':
			is_special = (*fp == '[');
			break;
			case '{':
			case '}':
			is_highlighted = (*fp == '{');
			break;
			case '|':
			is_underlined = !is_underlined;
			break;
			default:
			text_buf[cur_screen][i][j] = ((*fp) | (is_underlined ? 0x80 : 0x00));
			text_color_buf[cur_screen][i][j] = is_special     ? curr_text_color_sp   : curr_text_color;
			bg_color_buf[cur_screen][i++][j] = is_highlighted ? curr_highlight_color : curr_bg_color;
			break;
		}
		fp++;
	}
}

/**
  * @brief  Print a string at certain position, use [] to indicate special words
  * @param  x: starting x-coordinate
  * @param  y: starting y-coordinate
  * @param  pstr: string to be printed
  */
void tft_prints(uint8_t x, uint8_t y, const char * fmt, ...) {
	char buf[255]={0}, *fp=buf;
	
	va_list args;
	va_start( args, fmt );
	int c = print( &fp, fmt, args );

	tft_printc(x, y, buf);
}

void tft_printi(uint8_t x, uint8_t y, int32_t num) {
	char buf[12]={0};

	itos(buf, num);

	tft_printc(x, y, buf);
}

void tft_printl(uint8_t x, uint8_t y, int64_t num) {
	char buf[12]={0};

	ltos(buf, num);

	tft_printc(x, y, buf);
}

void tft_printu(uint8_t x, uint8_t y, uint32_t num) {
	char buf[12]={0};

	utos(buf, num);

	tft_printc(x, y, buf);
}

void tft_printf(uint8_t x, uint8_t y, float num, uint32_t dp) {
	char buf[40];

	ftos(buf, num, dp);

	tft_printc(x, y, buf);
}

static inline bool tft_char_is_changed(uint8_t x, uint8_t y){
	// return true;
	return (text_buf[cur_screen][x][y] != text_buf[next_screen][x][y] \
		|| (text_color_buf[cur_screen][x][y] != text_color_buf[next_screen][x][y]) \
		|| (bg_color_buf[cur_screen][x][y] != bg_color_buf[next_screen][x][y]));
}

void tft_push_pxbuf(void* buf, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	while (HAL_SPI_GetState(&TFT_SPI) != HAL_SPI_STATE_READY);
	
	tft_set_region(x, y, w-1, h-1);
	
	tft_write_many_dma(0x2c, buf, w*h*2);
}

#define CHAR_BYTES            CHAR_WIDTH*CHAR_HEIGHT*2
#define THREADED_TFT_BUF_LEN  CHAR_MAX_X
void tft_update(void) {
	uint8_t buf_ptr = 0;
	static uint8_t tft_dma_bufs[2][CHAR_BYTES * THREADED_TFT_BUF_LEN] = {0};
	
	for (uint8_t j = 0; j < char_max_y; j++) {
		for (uint8_t i = 0; i < char_max_x; i++) {
			if (tft_char_is_changed(i, j)) {
				uint8_t adj_cnt = 1;
				while ((i + adj_cnt < char_max_x) && tft_char_is_changed(i + (adj_cnt), j) && (adj_cnt < THREADED_TFT_BUF_LEN))
					adj_cnt++; //Search for diff char

				uint16_t px = 0;
				uint16_t y = 0;
				while (y < CHAR_HEIGHT) {
					uint8_t char_cnt = 0;
					while (char_cnt != adj_cnt) {
						const uint8_t char_ptr = (CHAR_PTR(text_buf[cur_screen][i + char_cnt][j]))[y];
						uint16_t fg = text_color_buf[cur_screen][i + char_cnt][j];
						uint16_t bg = bg_color_buf[cur_screen][i + char_cnt][j];
						for (uint8_t x = CHAR_WIDTH; x > 0; x--) {
							uint16_t color = ((char_ptr >> (x - 1)) & 0x01) ? fg : bg;
							tft_dma_bufs[buf_ptr][px++] = color >> 8;
							tft_dma_bufs[buf_ptr][px++] = color;
						}
						char_cnt++;
					}
					y++;
				}

				while (HAL_SPI_GetState(&TFT_SPI) != HAL_SPI_STATE_READY);
				
				tft_set_region(i * CHAR_WIDTH, j * CHAR_HEIGHT, adj_cnt * CHAR_WIDTH - 1, CHAR_HEIGHT-1);
				
				//Start next DMA Transmission
				// tft_write_many_dma(0x2c, &tft_dma_bufs[buf_ptr][0], CHAR_BYTES * adj_cnt);
				tft_write_many(0x2c, &tft_dma_bufs[buf_ptr][0], CHAR_BYTES * adj_cnt);
				
				buf_ptr ^= 0x1;
				i += adj_cnt - 1;
			}
		}
	}

	// Swap pointers
	cur_screen = (cur_screen == 0);
	next_screen = (next_screen == 0);
}

#define tft_px_index(py, px) (tft_px_buffer[px + py*row_width])
void tft_pxbuf_update(uint16_t* tft_px_buffer) {
	const uint8_t *char_ptr;
	uint16_t color;
	uint32_t px = 0, py = 0;
	uint16_t* pix;
	
	for (uint32_t j = 0; j < char_max_y; j++) {
		for (uint32_t i = 0; i < char_max_x; i++) {
			if (tft_char_is_changed(i, j)) {
				px = i * CHAR_WIDTH;
				py = j * CHAR_HEIGHT;
				pix = &tft_px_index(py, px);
				char_ptr = CHAR_PTR(text_buf[cur_screen][i][j]);
				uint16_t fg = text_color_buf[cur_screen][i][j];
				uint16_t bg = bg_color_buf[cur_screen][i][j];
				for (uint32_t y = 0; y < CHAR_HEIGHT; y++) {
					char_ptr++;
					for (uint32_t x = CHAR_WIDTH; x > 0; x--) {
						color = (*char_ptr >> (x - 1) & 0x01) ? fg : bg;
						*pix++ = color;
					}
					pix += row_width-CHAR_WIDTH;
				}
			}
		}
	}
	
	cur_screen = (cur_screen == 0);
	next_screen = (next_screen == 0);
}
void tft_pxbuf_write(uint16_t* tft_px_buffer) {
	const uint8_t *char_ptr;
	uint16_t color;
	uint32_t px = 0, py = 0;
	uint16_t* pix;
	
	for (uint32_t j = 0; j < char_max_y; j++) {
		for (uint32_t i = 0; i < char_max_x; i++) {
			char txt = text_buf[cur_screen][i][j];
			if (txt != ' ') {
				px = i * CHAR_WIDTH;
				py = j * CHAR_HEIGHT;
				pix = &tft_px_index(py, px);
				char_ptr = CHAR_PTR(txt);
				uint16_t fg = text_color_buf[cur_screen][i][j];
				uint16_t bg = bg_color_buf[cur_screen][i][j];
				for (uint32_t y = 0; y < CHAR_HEIGHT; y++) {
					char_ptr++;
					for (uint32_t x = CHAR_WIDTH; x > 0; x--) {
						color = (*char_ptr >> (x - 1) & 0x01) ? fg : bg;
						*pix++ = color;
					}
					pix += row_width-CHAR_WIDTH;
				}
			}
		}
	}
	
	cur_screen = (cur_screen == 0);
	next_screen = (next_screen == 0);
}

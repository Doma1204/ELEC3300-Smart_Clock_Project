#ifndef _XPT2046_H
#define _XPT2046_H

#include "spi.h"
#include "gpio.h"
#include "led.h"
#include "tft.h"
#include "buffer.h"

#define SELECT_X 0x90 // X-axis measurement
#define SELECT_Y 0xD0 // Y-axis measurement
#define SELECT_P 0xB0 // Pressure measurement

#define CTRL_SER 0x03 // Single-ended mode
#define CTRL_DFR 0x04 // Differential Reference Mode

#define XPT_SPI &hspi1

void touch_interrupt_handler(void);
void xpt_readRawValue(uint8_t mode, uint8_t maxSample);

uint16_t xpt_getRawX(void);
uint16_t xpt_getRawY(void);

#endif

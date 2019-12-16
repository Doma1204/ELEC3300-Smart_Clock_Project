#ifndef _PANEL_POSITIONING_H
#define _PANEL_POSITIONING_H

#include <stdbool.h>
#include "stm32f0xx_hal.h"
#include "gpio.h"
#include "i2C.h"
// #include "usart.h"

#define short_time 2
#define long_time 3
#define stop_time 4
#define emit_times 5

typedef enum _DIRECTION {
    TOP = 0,
    LEFT,
    BOTTOM,
    RIGHT,
    NONE
} Direction;

extern Direction dirFrom, orientation;
extern volatile uint8_t panelCount;
extern bool isDetect;
extern volatile bool isSearch;

#define getDirection() dirFrom
#define getOrientation() orientation
#define getPanelCount() panelCount
#define isDetectingPanel() isDetect
#define isSearchingPanel() isSearch

void panelCountInc(void);
void resumeSearch(void);

void startDetectPanel(void);
void stopDetectPanel(void);
void startSearch_master(void);

void detectPanel(Direction d);

#endif

#ifndef _PIR_H
#define _PIR_H

#include "gpio.h"

extern volatile uint8_t pir;

#define getPIR() pir

void PIR_init(void);
void PIR_interrupt_handle(void);

#endif

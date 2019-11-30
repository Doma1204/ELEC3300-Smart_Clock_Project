#include "pir.h"

volatile uint8_t pir = 0;

void PIR_init(void) {pir = HAL_GPIO_ReadPin(PIR_GPIO_Port, PIR_Pin);}
void PIR_interrupt_handle(void) {pir = HAL_GPIO_ReadPin(PIR_GPIO_Port, PIR_Pin);}

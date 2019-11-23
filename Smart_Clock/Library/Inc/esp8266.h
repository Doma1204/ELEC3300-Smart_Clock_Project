#ifndef _ESP8266_H
#define _ESP8266_H

#include "main.h"
#include "led.h"
#include "button.h"
#include "usart.h"

// #define GET_STATUS_CMD 0x80
// #define GET_STATUS_LENGTH 3

// #define GET_IP_CMD 0x81
// #define GET_IP_LENGTH 10

void esp8266_init(void);
void reset_esp(void);
void enter_esp_flashing_mode(void);
void exit_esp_flashing_mode(void);
uint8_t is_esp_flashing(void);

#endif

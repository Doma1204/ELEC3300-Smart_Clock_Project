#ifndef _ESP8266_H
#define _ESP8266_H

#include <string.h>
#include "main.h"
#include "led.h"
#include "button.h"
#include "usart.h"
#include "buffer.h"

#define ESP_UART &huart6
#define ESP_TX_BUFF_LENGTH 4
#define ESP_RX_BUFF_LENGTH 10

#define 

#define GET_STATUS_CMD 0x80
#define GET_STATUS_LENGTH 3

#define GET_IP_CMD 0x81
#define GET_IP_LENGTH 10

#define GET_TIME_CMD 0x54
#define GET_TIME_CMD_LENGTH 2
#define GET_TIME_DATA_LENGTH 7

// void esp8266_init(void);
// void reset_esp(void);
// void enter_esp_flashing_mode(void);
// void exit_esp_flashing_mode(void);
// uint8_t is_esp_flashing(void);

extern uint8_t rx_buff[ESP_RX_BUFF_LENGTH];

void requestTime(void);
uint32_t getTime(void);

#endif

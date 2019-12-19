#ifndef _ESP8266_H
#define _ESP8266_H

#include <string.h>
#include "main.h"
#include "led.h"
#include "button.h"
#include "usart.h"
#include "epochToNormal.h"
#include "buffer.h"

#define ESP_UART &huart1
#define ESP_TX_BUFF_LENGTH 4
#define ESP_RX_BUFF_LENGTH 30

#define GET_STATUS_CMD         0x52
#define GET_STATUS_CMD_LENGTH  2
#define GET_STATUS_DATA_LENGTH 3

#define GET_TIME_CMD         0x54
#define GET_TIME_CMD_LENGTH  2
#define GET_TIME_DATA_LENGTH 7

#define GET_TEMP_CMD 0x43
#define GET_TEMP_CMD_LENGTH 3
#define GET_TEMP_DATA_LENGTH 4 //FORECAST_DAY*3+2

#define FORECAST_DAY 3
#define GET_FORECAST_CMD 0x46
#define GET_FORECAST_CMD_LENGTH 2
#define GET_FORECAST_DATA_LENGTH FORECAST_DAY * 3 + 3

#define ESP_UPDATE_FREQUENCY 1000
#define ESP_TIME_UPDATE_FREQUENCY 300000
#define ESP_TEMPERATURE_UPDATE_FREQUENCY 3600000
#define ESP_FORECAST_UPDATE_FREQUENCY 3600000

typedef enum _LOCATION {
    Kings_Park,
    Hong_Kong_Observatory,
    Wong_Chuk_Hang,
    Ta_Kwu_Ling,
    Lau_Fau_Shan,
    Tai_Po,
    Sha_Tin,
    Tuen_Mun,
    Tseung_Kwan_O,
    Sai_Kung,
    Cheung_Chau,
    Chek_Lap_Kok,
    Tsing_Yi,
    Shek_Kong,
    Tsuen_Wan_Ho_Koon,
    Tsuen_Wan_Shing_Mun_Valley,
    Hong_Kong_Park,
    Shau_Kei_Wan,
    Kowloon_City,
    Happy_Valley,
    Wong_Tai_Sin,
    Stanley,
    Kwun_Tong,
    Sham_Shui_Po,
    Kai_Tak_Runway_Park,
    Yuen_Long_Park,
    Tai_Mei_Tuk
} LOCATION;

static const char* Location_str[27] = {
    "King's Park",
    "Hong Kong Observatory",
    "Wong Chuk Hang",
    "Ta Kwu Ling",
    "Lau Fau Shan",
    "Tai Po",
    "Sha Tin",
    "Tuen Mun",
    "Tseung Kwan O",
    "Sai Kung",
    "Cheung Chau",
    "Chek Lap Kok",
    "Tsing Yi",
    "Shek Kong",
    "Tsuen Wan Ho Koon",
    "Tsuen Wan Shing Mun Valley",
    "Hong Kong Park",
    "Shau Kei Wan",
    "Kowloon City",
    "Happy Valley",
    "Wong Tai Sin",
    "Stanley",
    "Kwun Tong",
    "Sham Shui Po",
    "Kai Tak Runway Park",
    "Yuen Long Park",
    "Tai Mei Tuk"
};

extern uint8_t rx_buff[ESP_RX_BUFF_LENGTH];

void esp_update(void);
void requestStatus(void);
void requestTime(void);
void requestTemperature(LOCATION location);
void requestForecast(void);
#endif

#include "esp8266.h"

static uint8_t tx_buff[ESP_TX_BUFF_LENGTH];
uint8_t rx_buff[ESP_RX_BUFF_LENGTH];
uint8_t data_length = 0;

uint8_t status = 0;
uint8_t cur_location = 0;
uint8_t cur_temp = 0;
uint8_t cur_icon = 0;
uint8_t fore_icon[FORECAST_DAY], fore_max[FORECAST_DAY], fore_min[FORECAST_DAY];

uint32_t last_get_time_ticks = 0; 
uint32_t last_get_temperature_ticks  = 0; 
uint32_t last_get_forecaset_ticks  = 0; 

void esp_update(void) {
	static uint32_t last_tft_ticks = 0;
    uint32_t this_tick = HAL_GetTick();
    if (this_tick - last_tft_ticks < ESP_UPDATE_FREQUENCY) return;
    last_tft_ticks = this_tick;

    if (!status) {
        requestStatus();
        return;
    }
    
    if (HAL_GetTick() - this_tick >= ESP_TIME_UPDATE_FREQUENCY || !last_get_time_ticks) {
        requestTime();
        return;
    }

    if (HAL_GetTick() - this_tick >= ESP_TEMPERATURE_UPDATE_FREQUENCY || !last_get_temperature_ticks ) {
        requestTemperature((LOCATION) cur_location);
        return;
    }

    if (HAL_GetTick() - this_tick >= ESP_FORECAST_UPDATE_FREQUENCY || !last_get_forecaset_ticks ) {
        requestForecast();
        return;
    }
}

__forceinline void shiftArray(uint8_t* arr, uint8_t size, uint8_t shift) {
    for (uint8_t i = 0; i < size; ++i) {
        arr[i] = arr[i+shift];
    }
}

void esp_recieve_handle(void) {
    if (rx_buff[0] != '\r') {
        uint8_t i;
        for (i = 1; i < data_length; ++i)
            if (rx_buff[i] == '\r') break;
        
        uint8_t length = data_length - i;
        shiftArray(rx_buff, length - i, i);
        HAL_UART_Receive(ESP_UART, &rx_buff[length - i], length, 100);
    }

    switch (rx_buff[1]) {
        case GET_STATUS_CMD: {
            status = rx_buff[2];
        }

        case GET_TIME_CMD: {
            uint32_t curTime = 0;
            curTime |= rx_buff[5] << 24;
            curTime |= rx_buff[4] << 16;
            curTime |= rx_buff[3] << 8;
            curTime |= rx_buff[2];
            setTime(curTime);
            last_get_time_ticks = HAL_GetTick();
            break;
        }

        case GET_TEMP_CMD: {
            cur_temp = rx_buff[2];
            cur_icon = rx_buff[3];
            last_get_temperature_ticks = HAL_GetTick();
            break;
        }

        case GET_FORECAST_CMD: {
            memcpy(fore_icon, &rx_buff[2], FORECAST_DAY);
            memcpy(fore_min, &rx_buff[2+FORECAST_DAY], FORECAST_DAY);
            memcpy(fore_max, &rx_buff[2+FORECAST_DAY*2], FORECAST_DAY);
            last_get_forecaset_ticks = HAL_GetTick();
            break;
        }

        default:
            break;
    }
}

void requestStatus(void) {
    tx_buff[0] = GET_STATUS_CMD;
    tx_buff[1] = '\n';
    HAL_UART_Transmit_IT(ESP_UART, tx_buff, GET_STATUS_CMD_LENGTH);
    HAL_UART_Receive_IT(ESP_UART, rx_buff, GET_TIME_DATA_LENGTH);
    data_length = GET_STATUS_DATA_LENGTH;
}

void requestTime(void) {
    tx_buff[0] = GET_TIME_CMD;
    tx_buff[1] = '\n';
    HAL_UART_Transmit_IT(ESP_UART, tx_buff, GET_TIME_CMD_LENGTH);
    HAL_UART_Receive_IT(ESP_UART, rx_buff, GET_TIME_DATA_LENGTH);
    data_length = GET_TIME_DATA_LENGTH;
}

void requestTemperature(LOCATION location) {
    tx_buff[0] = GET_TEMP_CMD;
    tx_buff[1] = location;
    tx_buff[2] = '\n';
    cur_location = location;
    HAL_UART_Transmit_IT(ESP_UART, tx_buff, GET_TEMP_CMD_LENGTH);
    HAL_UART_Receive_IT(ESP_UART, rx_buff, GET_TEMP_DATA_LENGTH);
    data_length = GET_TEMP_DATA_LENGTH;
}

void requestForecast(void) {
    tx_buff[0] = GET_FORECAST_CMD;
    tx_buff[1] = '\n';
    HAL_UART_Transmit_IT(ESP_UART, tx_buff, GET_FORECAST_CMD_LENGTH);
    HAL_UART_Receive_IT(ESP_UART, rx_buff, GET_FORECAST_DATA_LENGTH);
    data_length = GET_FORECAST_DATA_LENGTH;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == ESP_UART) {
        esp_recieve_handle();
    }
}

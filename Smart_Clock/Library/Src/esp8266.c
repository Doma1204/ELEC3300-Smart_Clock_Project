#include "esp8266.h"

// uint8_t isFlashing = 0;

// void esp8266_init(void) {
//     GPIO_InitTypeDef GPIO_InitStruct = {0};

//     HAL_GPIO_WritePin(ESP_EN_GPIO_Port, ESP_EN_Pin, GPIO_PIN_RESET);
//     HAL_GPIO_WritePin(ESP_IO0_GPIO_Port, ESP_IO0_Pin, GPIO_PIN_SET);

//     GPIO_InitStruct.Pin = ESP_RST_Pin;
//     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//     HAL_GPIO_Init(ESP_RST_GPIO_Port, &GPIO_InitStruct);

//     GPIO_InitStruct.Pin = ESP_EN_Pin | ESP_IO0_Pin;
//     GPIO_InitStruct.Pin = ESP_EN_Pin;
//     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//     HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

//     void toggle_esp_flashing_mode(void);
//     set_button_onHoldListener(BUTTON1, 20, 0, toggle_esp_flashing_mode);
// }

// void toggle_esp_flashing_mode(void) {
//     if (isFlashing) exit_esp_flashing_mode();
//     else enter_esp_flashing_mode();
// }

// void reset_esp(void) {
//     HAL_GPIO_WritePin(ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_RESET);
//     HAL_Delay(1);
//     HAL_GPIO_WritePin(ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_SET);
// }

// void enter_esp_flashing_mode(void) {
//     HAL_GPIO_WritePin(ESP_EN_GPIO_Port, ESP_EN_Pin, GPIO_PIN_SET);
//     HAL_GPIO_WritePin(ESP_IO0_GPIO_Port, ESP_IO0_Pin, GPIO_PIN_RESET);
//     reset_esp();

//     isFlashing = 1;
//     led_on(LED3);
// }

// void exit_esp_flashing_mode(void) {
//     HAL_GPIO_WritePin(ESP_EN_GPIO_Port, ESP_EN_Pin, GPIO_PIN_RESET);
//     HAL_GPIO_WritePin(ESP_IO0_GPIO_Port, ESP_IO0_Pin, GPIO_PIN_SET);
//     reset_esp();

//     isFlashing = 1;
//     led_off(LED3);
// }

// uint8_t is_esp_flashing(void) {return isFlashing;}

static uint8_t tx_buff[ESP_TX_BUFF_LENGTH];
uint8_t rx_buff[ESP_RX_BUFF_LENGTH];
uint8_t data_length = 0;
static uint32_t curTime = 0;

__forceinline void shiftArray(uint8_t* arr, uint8_t size, uint8_t shift) {
    for (uint8_t i = 0; i < size; ++i) {
        arr[i] = arr[i+shift];
    }
}

void esp_recieve_handle(void) {
    // switch (rx_buff[0]) {

    // }
    // BUFFER32 time;
    // memcpy(&rx_buff[1], time.buffer, 4);

    if (rx_buff[0] != '\r') {
        uint8_t i;
        for (i = 1; i < data_length; ++i)
            if (rx_buff[i] == '\r') break;
        
        uint8_t length = data_length - i;
        shiftArray(rx_buff, length - i, i);
        HAL_UART_Receive(ESP_UART, &rx_buff[length - i], length, 100);
    }

    switch (rx_buff[1]) {
        case GET_TIME_CMD:
            curTime = 0;
            curTime |= rx_buff[5] << 24;
            curTime |= rx_buff[4] << 16;
            curTime |= rx_buff[3] << 8;
            curTime |= rx_buff[2];
            break;

        default:
            break;
    }
}

uint32_t getTime(void) {return curTime;}

void requestTime(void) {
    led_toggle(LED3);
    tx_buff[0] = GET_TIME_CMD;
    tx_buff[1] = '\n';
    HAL_UART_Transmit(ESP_UART, tx_buff, GET_TIME_CMD_LENGTH, 100);
    HAL_UART_Receive(ESP_UART, rx_buff, GET_TIME_DATA_LENGTH, 100);
    data_length = GET_TIME_CMD_LENGTH;
    esp_recieve_handle();
}

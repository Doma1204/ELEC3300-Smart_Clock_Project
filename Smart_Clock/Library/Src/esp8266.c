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


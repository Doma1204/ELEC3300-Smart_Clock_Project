#include "xpt2046.h"

volatile uint16_t x, y, p;

void touch_interrupt_handler(void) {
    if (HAL_GPIO_ReadPin(TFT_PEN_GPIO_Port, TFT_PEN_Pin)) { // On Release
        led_off(LED2); // debug
    } else { // On Press
        led_on(LED2); // debug
        xpt_readRawValue(CTRL_SER, 3);
        // tft_printi(0, 0, HAL_GetTick());
        // tft_prints(0, 1, "X: %d", x);
        // tft_prints(0, 2, "Y: %d", y);
        // tft_update();
    }
}

#define CS_HIGH() TFT_CS2_GPIO_Port->BSRR = TFT_CS2_Pin
#define CS_LOW()  TFT_CS2_GPIO_Port->BSRR = (uint32_t) TFT_CS2_Pin << 16U
#define XPT_T(ptr, size) HAL_SPI_Transmit(XPT_SPI, ptr, size, 0xffff)
#define XPT_TR(tx_ptr, rx_ptr, size) HAL_SPI_TransmitReceive(XPT_SPI, tx_ptr, rx_ptr, size, 0xffff)

uint16_t read(uint8_t ctrl_byte, uint8_t maxSample) {
    // BUFFER16 cur = {0xff, 0xff}, prev = {0xff, 0xff};
    // BUFFER16 cmd = {0, ctrl_byte};
    // uint8_t i = 0;

    // do {
    //     prev = cur;
    //     XPT_TR(cmd.buffer, cur.buffer, 2);
    //     cur.value >>= 4;
    // } while ((prev.value != cur.value) && (++i < maxSample));
    // return cur.value;

    uint16_t cur = 0xffff, prev = 0xffff;
    uint8_t cmd[2] = {0, ctrl_byte};
    uint8_t i = 0;

    uint8_t buffer[2] = {0};
    do {
        prev = cur;
        XPT_TR(cmd, buffer, 2);
        cur = ((uint16_t) buffer[0] << 4) | (buffer[1] >> 4);
    } while ((prev != cur) && (++i < maxSample));

    return cur;
}

void xpt_readRawValue(uint8_t mode, uint8_t maxSample) {
    if (mode != CTRL_SER || mode != CTRL_DFR) return;

    uint8_t ctrl_byte = SELECT_X | mode;
    
    // HAL_GPIO_WritePin(TFT_CS1_GPIO_Port, TFT_CS1_Pin, GPIO_PIN_SET);
    CS_LOW();
    XPT_T(&ctrl_byte, 1);
    x = read(SELECT_X | mode, maxSample);
    y = read(SELECT_Y | mode, maxSample);
    
    BUFFER16 buff;
    if (mode == CTRL_DFR) {
        buff.value = SELECT_Y | CTRL_SER;
        XPT_T(buff.buffer, 2);
    }

    buff.value = 0;
    XPT_T(buff.buffer, 2);

    CS_HIGH();
    // HAL_GPIO_WritePin(TFT_CS1_GPIO_Port, TFT_CS1_Pin, GPIO_PIN_RESET);
}

uint16_t xpt_getRawX(void) {return x;}
uint16_t xpt_getRawY(void) {return y;}

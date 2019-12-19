#include "gy39.h"

static uint32_t lux, pressure;
static uint16_t temperature, humidity, altitude;
// static uint16_t temp = 100;

#define get_register(addr, data) HAL_I2C_Mem_Read(GY39_I2C, GY39_ADDR, addr, 1, &data, 1, 100)

void gy39_update(void) {
    static uint32_t last_gy39_ticks = 0;
    uint32_t this_ticks = HAL_GetTick();
    if (this_ticks - last_gy39_ticks < GY39_UPDATE_FREQUENCY || !last_gy39_ticks) return;
    last_gy39_ticks = this_ticks;

    uint8_t buff[4];

    get_register(H_LUX_H, buff[0]);
    get_register(H_LUX_L, buff[1]);
    get_register(L_LUX_H, buff[2]);
    get_register(L_LUX_L, buff[3]);
    lux = buff[0] << 24 | buff[1] << 16 | buff[2] << 8 | buff[3];

    get_register(TEMP_H, buff[0]);
    get_register(TEMP_L, buff[1]);
    temperature = buff[0] << 8 | buff[1];

    get_register(H_PRE_H, buff[0]);
    get_register(H_PRE_L, buff[1]);
    get_register(L_PRE_H, buff[2]);
    get_register(L_PRE_L, buff[3]);
    pressure = buff[0] << 24 | buff[1] << 16 | buff[2] << 8 | buff[3];

    get_register(HUM_H, buff[0]);
    get_register(HUM_L, buff[1]);
    humidity = buff[0] << 8 | buff[1];

    get_register(ALT_H, buff[0]);
    get_register(ALT_L, buff[1]);
    altitude = buff[0] << 8 | buff[1];
}

uint32_t gy39_getLux(void) {return lux;}
uint32_t gy39_getPressure(void) {return pressure;}
uint16_t gy39_getTemperature(void) {return temperature;}
uint16_t gy39_getHumidity(void) {return humidity;}
uint16_t gy39_getAltitude(void) {return altitude;}

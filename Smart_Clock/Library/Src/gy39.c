#include "gy39.h"

static BUFFER32 lux, pressure;
static BUFFER16 temperature, humidity, altitude;
static uint16_t temp = 100;

#define get_register(addr, data) HAL_I2C_Mem_Read(GY39_I2C, GY39_ADDR, addr, 1, &data, 1, 10)

void gy39_update(void) {
    static uint32_t last_gy39_ticks = 0;
    uint32_t this_ticks = HAL_GetTick();
    if (this_ticks - last_gy39_ticks < GY39_UPDATE_FREQUENCY) return;
    last_gy39_ticks = this_ticks;

    // get_register(H_LUX_H, lux.buffer[0]);
    // get_register(H_LUX_L, lux.buffer[1]);
    // get_register(L_LUX_H, lux.buffer[2]);
    // get_register(L_LUX_L, lux.buffer[3]);

    // get_register(TEMP_H, temperature.buffer[0]);
    // get_register(TEMP_L, temperature.buffer[1]);

    uint8_t t;
    temp = 0;
    get_register(TEMP_H, t);
    temp = (uint16_t) t << 8;
    get_register(TEMP_L, t);
    temp |= t;

    // get_register(H_PRE_H, pressure.buffer[0]);
    // get_register(H_PRE_L, pressure.buffer[1]);
    // get_register(L_PRE_H, pressure.buffer[2]);
    // get_register(L_PRE_L, pressure.buffer[3]);

    // get_register(HUM_H, humidity.buffer[0]);
    // get_register(HUM_L, humidity.buffer[1]);

    // get_register(ALT_H, altitude.buffer[0]);
    // get_register(ALT_L, altitude.buffer[1]);
}

uint32_t gy39_getLux(void) {return lux.value;}
uint32_t gy39_getPressure(void) {return pressure.value;}
uint16_t gy39_getTemperature(void) {return temp;}
// uint16_t gy39_getTemperature(void) {return temperature.value;}
uint16_t gy39_getHumidity(void) {return humidity.value;}
uint16_t gy39_getAltitude(void) {return altitude.value;}

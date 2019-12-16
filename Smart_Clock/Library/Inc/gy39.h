#ifndef _GY39_H
#define _GY39_H

#include "i2c.h"
#include "buffer.h"

#define GY39_UPDATE_FREQUENCY 1000

#define GY39_I2C &hi2c1

// I2C address
#define GY39_ADDR 0x5B

#define H_LUX_H 0x00 // Lux bit 24-31
#define H_LUX_L 0x01 // Lux bit 16-23
#define L_LUX_H 0x02 // Lux bit 8-15
#define L_LUX_L 0x03 // Lux bit 0-7
#define TEMP_H  0x04 // Temperature bit 8-15
#define TEMP_L  0x05 // Temperature bit 0-7
#define H_PRE_H 0x06 // Pressure bit 24-31
#define H_PRE_L 0x07 // Pressure bit 16-23
#define L_PRE_H 0x08 // Pressure bit 8-15
#define L_PRE_L 0x09 // Pressure bit 0-7
#define HUM_H   0x0A // Humidity bit 8-15
#define HUM_L   0x0B // Humidity bit 0-7
#define ALT_H   0x0C // Altitude bit 8-15
#define ALT_L   0x0D // Altitude bit 0-7

void gy39_update(void);

uint32_t gy39_getLux(void);
uint32_t gy39_getPressure(void);
uint16_t gy39_getTemperature(void);
uint16_t gy39_getHumidity(void);
uint16_t gy39_getAltitude(void);

#endif

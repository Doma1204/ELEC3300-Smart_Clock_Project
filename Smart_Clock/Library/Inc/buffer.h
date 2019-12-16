#ifndef _BUFFER_H
#define _BUFFER_H

typedef union _BUFFER16 {
    uint8_t buffer[2];
    uint16_t value;
} BUFFER16;

typedef union _BUFFER32 {
    uint8_t buffer[4];
    uint32_t value;
} BUFFER32;

#endif

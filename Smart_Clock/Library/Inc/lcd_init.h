#ifndef __TFT_INIT_H
#define __TFT_INIT_H

#define TFT_ORIENTATION_CMD    0x36
#define TFT_EOF_MARKER         0x42
#define TFT_EOL_MARKER         0x43
#define TFT_DELAY_MARKER       0xFF

const uint8_t tft_init_code[] = { 0x00,

    0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,                     TFT_EOL_MARKER,
    0xCF, 0x00, 0XC1, 0X30,                                 TFT_EOL_MARKER,
    0xE8, 0x85, 0x00, 0x78,                                 TFT_EOL_MARKER,
    0xEA, 0x00, 0x00,                                       TFT_EOL_MARKER,
    0xEA, 0x00, 0x00,                                       TFT_EOL_MARKER,
    0xED, 0x64, 0x03, 0x12, 0x81,                           TFT_EOL_MARKER,
    0xF7, 0x20,                                             TFT_EOL_MARKER,
    // Power control VRH[5:0] = 4.60 V
    0xC0, 0x23,                                             TFT_EOL_MARKER,
    // Power control SAP[2:0];BT[3:0] = adjust power to lowest possible
    0xC1, 0x10,                                             TFT_EOL_MARKER,
    // VCM control 1 Contrast VCOMH = 4.250 VCOML = -1.500
    0xC5, 0x3e, 0x28,                                       TFT_EOL_MARKER,
    // VCM control 2
    0xC7, 0x86,                                             TFT_EOL_MARKER,
    // Memory Access Control - 0x08 = BGR color filter panel, 0x40 = Column Address Order 1
    TFT_ORIENTATION_CMD,                                    TFT_EOL_MARKER,
    // COLMOD: Pixel Format Set 16bits for pixel
    0x3A, 0x55,                                             TFT_EOL_MARKER,
    // Frame Rate Control (In Normal Mode/Full Colors) - division ratio: 1 - 24 clocks
    0xB1, 0x00, 0x18,                                       TFT_EOL_MARKER,
    // Display Function Control
    0xB6, 0x08, 0x82, 0x27,                                 TFT_EOL_MARKER,
    // Exit sleep
    0x11, TFT_DELAY_MARKER, 120,                            TFT_EOL_MARKER,
    0x29,                                                   TFT_EOL_MARKER,
    0x2c,                                                   TFT_EOL_MARKER,

    // EOF - not sent to controller
    TFT_EOF_MARKER
};
const uint8_t tft_orientation_init_code[] = {
    0x48,
    0x28,
    0x88,
    0xE8,
};

#endif

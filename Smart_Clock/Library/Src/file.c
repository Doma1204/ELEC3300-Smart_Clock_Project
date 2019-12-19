#include "file.h"

static FATFS SD_Card;
static uint8_t is_card_exist = 0;

FRESULT sd_card_init(void) {
    FRESULT res = f_mount(&SD_Card, SDPath, 1);
    is_card_exist = res == FR_OK ? 1 : 0;
    return res;
}

FRESULT read_file_to_buffer(const char* fileName, uint8_t* buff, UINT* numOfBytes) {
    FRESULT res;
    if (!is_card_exist) {
        res = sd_card_init();
        if (res != FR_OK) return res;
    }

    FIL file;
    res = f_open(&file, fileName, FA_READ | FA_OPEN_EXISTING);
    if (res != FR_OK) return res;

    res = f_read(&file, buff, *numOfBytes, numOfBytes);
    if(res != FR_OK) return res;

    f_close(&file);
}

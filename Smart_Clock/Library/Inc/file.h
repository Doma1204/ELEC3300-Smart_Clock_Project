#ifndef _FILE_H
#define _FILE_H

#include "fatfs.h"

FRESULT sd_card_init(void);
FRESULT read_file_to_buffer(const char* fileName, uint8_t* buff, UINT* numOfBytes);

#endif

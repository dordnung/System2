#ifndef _CRC_H
#define _CRC_H

#include "extension.h"
#include <sstream>

uint32_t updateCRC32(unsigned char ch, uint32_t crc);
bool crc32file(char *name, uint32_t *crc, long *charcnt);
uint32_t crc32buf(char *buf, size_t len);
void crc32ToHex(uint32_t crc, char *buf, size_t len);

#endif
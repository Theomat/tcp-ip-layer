#include <stdint.h>

#ifndef PROTOCOLS_H
#define PROTOCOLS_H
uint16_t sum16(const void* addr, unsigned int count);
#define internet_checksum(addr, count) ((uint16_t)~sum16(addr, count))
#endif

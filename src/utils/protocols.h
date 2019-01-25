#include <stdint.h>

#ifndef PROTOCOLS_H
#define PROTOCOLS_H
/*
 * Taken from https://tools.ietf.org/html/rfc1071
 */
uint16_t internet_checksum(const void* addr, unsigned int count);
#endif

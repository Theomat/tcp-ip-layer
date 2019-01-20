#ifndef CHECKSUM_H
#define CHECKSUM_H
/*
 * Taken from https://tools.ietf.org/html/rfc1071
 */
uint16_t internet_checksum(void* addr, int count);
#endif

#include <stdbool.h>
#include <stdint.h>
#ifndef TUNTAP_INTERFACE_H
#define TUNTAP_INTERFACE_H

#ifndef TUN_QUEUES
#define TUN_QUEUES 10
#endif
//------------------------------------------------------------------------------
//                               IO
//------------------------------------------------------------------------------
int tun_read(int8_t fi, char* buf, int len);
int tun_write(int8_t fi, char* buf, int len);
//------------------------------------------------------------------------------
//                               STATUS
//------------------------------------------------------------------------------
void set_busy(uint8_t fi, bool busy);
int8_t available();
//------------------------------------------------------------------------------
//                           ALLOCATION
//------------------------------------------------------------------------------
void tun_init(char* dev);

#endif

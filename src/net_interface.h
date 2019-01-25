#include <stdint.h>

#ifndef NET_INTERFACE_H
#define NET_INTERFACE_H
//------------------------------------------------------------------------------
//                               GETTERS
//------------------------------------------------------------------------------
uint32_t net_interface_get_ip();
unsigned char* net_interface_get_mac();
//------------------------------------------------------------------------------
//                           SEND DATA
//------------------------------------------------------------------------------
void net_interface_send(const char* content, uint32_t len,
                        const unsigned char* dst_mac, uint16_t protocol);
void net_interface_broadcast(const char* content, uint32_t len,
                             uint16_t protocol);
//------------------------------------------------------------------------------
//                           RECEIVE DATA
//------------------------------------------------------------------------------
int net_interface_read(char* buffer, uint32_t len);
//------------------------------------------------------------------------------
//                           ALLOCATION
//------------------------------------------------------------------------------
void net_interface_init(const char* ip, const char* mac);
//------------------------------------------------------------------------------
//                           LIFECYCLE
//------------------------------------------------------------------------------
void net_interface_destroy();

#endif

#ifndef NET_INTERFACE_H
#define NET_INTERFACE_H
struct net_interface;
//------------------------------------------------------------------------------
//                               GETTERS
//------------------------------------------------------------------------------
uint32_t net_interface_get_ip(struct net_interface* interface);
unsigned char* net_interface_get_mac(struct net_interface* interface);
//------------------------------------------------------------------------------
//                           SEND DATA
//------------------------------------------------------------------------------
void net_interface_send(struct net_interface* interface, char* content,
                        uint32_t len, unsigned char* dst_mac);
void net_interface_broadcast(struct net_interface* interface, char* content,
                             uint32_t len);
//------------------------------------------------------------------------------
//                           ALLOCATION
//------------------------------------------------------------------------------
struct net_interface* net_interface_alloc(char* ip, char* mac);
//------------------------------------------------------------------------------
//                           LIFECYCLE
//------------------------------------------------------------------------------
void net_interface_destroy();

#endif

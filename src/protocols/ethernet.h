#include <stdint.h>
#include <stdio.h>

#ifndef ETHERNET_H
#define ETHERNET_H
//------------------------------------------------------------------------------
//                                STRUCTURE
//------------------------------------------------------------------------------
struct eth_header {
  unsigned char dst_mac[6];
  unsigned char src_mac[6];
  uint16_t ether_type;
  // If ether_type >= 1536 : ether_type == payload type IPv4 or ARP
  // Else : ether_type == payload_length
  unsigned char payload[];
} __attribute__((packed));
//------------------------------------------------------------------------------
//                                PRINT
//------------------------------------------------------------------------------
void ethernet_fprint(struct eth_header* header, FILE* fd);
//------------------------------------------------------------------------------
//                                CONVERTION
//------------------------------------------------------------------------------
void eth_header_hton(struct eth_header* header);
void eth_header_ntoh(struct eth_header* header);

#endif

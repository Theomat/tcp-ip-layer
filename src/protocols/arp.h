#include <stdint.h>
#include <stdio.h>

#include "ethernet.h"

#ifndef ARP_H
#define ARP_H
//------------------------------------------------------------------------------
//                                  STRUCTURES
//------------------------------------------------------------------------------
#define ARP_ETHERNET 1
#define ARP_IPV4 0x0800
#define ARP_REQUEST 1
#define ARP_REPLY 2

struct arp_header {
  // link layer type used : Ethernet 0x0001
  uint16_t hw_type;
  // protocol type : IPv4 0x0800
  uint16_t pro_type;
  // MAC address size : 6Bytes | IPv4 address size : 4Bytes
  uint8_t hw_size;
  uint8_t pro_size;
  // Request : ARP request(1), ARP reply(2), RARP request(3), RARP reply(4)
  uint16_t opcode;
  unsigned char data[];
} __attribute__((packed));

struct arp_ipv4 {
  unsigned char src_mac[6];
  uint32_t src_ip;
  unsigned char dst_mac[6];
  uint32_t dst_ip;
} __attribute__((packed));
//------------------------------------------------------------------------------
//                                  PRINT
//------------------------------------------------------------------------------
void arp_header_fprint(struct arp_header* header, FILE* fd);
//------------------------------------------------------------------------------
//                              ARP LIFECYCLE
//------------------------------------------------------------------------------
// Allocate the transition table
void arp_init();
// Free transition table
void arp_destroy();
//------------------------------------------------------------------------------
//                             ARP RESOLVE
//------------------------------------------------------------------------------
void arp_receive(struct eth_header* eth_header);
// Returns the mac address if is in the arp cache
// If NOT returns NULL and send an ARP request packet to resolve the specific ip
unsigned char* arp_resolve_ipv4(uint32_t ipv4);

#endif

#ifndef ARP_H
#define ARP_H
//------------------------------------------------------------------------------
//                                  STRUCTURES
//------------------------------------------------------------------------------
#define ARP_ETHERNET 1
#define ARP_IPV4 0x0800
#define ARP_REQUEST 1
#define ARP_REPLY 2

struct arp_header;
struct arp_ipv4;
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
void arp_receive(struct net_interface* interface,
                 struct eth_header* eth_header);
// Returns the mac address if is in the arp cache
// If NOT returns NULL and send an ARP request packet to resolve the specific ip
unsigned char* arp_resolve_ipv4(struct net_interface* interface,
                                uint32_t ipv4);

#endif

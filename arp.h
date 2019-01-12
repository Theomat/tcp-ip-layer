#ifndef ARP_H
#define ARP_H

struct arp_header;
struct arp_ipv4;
void arp_init();
void arp_destroy();
void arp_resolution(struct net_interface* interface, struct arp_header* header);

#endif

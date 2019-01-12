#ifndef ARP_H
#define ARP_H
struct arp_header;
struct arp_ipv4;
void arp_init();
void arp_destroy();
void arp_resolve(struct net_interface* interface,
                 struct eth_header* eth_header);

#endif

#ifndef ETHERNET_H
#define ETHERNET_H
#define ETHERNET_HEADER_SIZE 16
struct eth_header;
void ethernet_fprint(struct eth_header* header, FILE* fd);
struct eth_header* to_ethernet_header(char* data);
struct eth_header* ethernet_header_alloc();
void ethernet_header_set(struct eth_header* header, uint16_t type,
                         char* payload, uint32_t len, unsigned char* src_mac,
                         unsigned char* dst_mac);
unsigned char* ethernet_payload(struct eth_header* header);
uint16_t ethernet_type(struct eth_header* header);

#endif

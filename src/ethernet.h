#ifndef ETHERNET_H
#define ETHERNET_H
#define ETHERNET_HEADER_SIZE 16
//------------------------------------------------------------------------------
//                              STRUCTURE
//------------------------------------------------------------------------------
struct eth_header;
//------------------------------------------------------------------------------
//                                PRINT
//------------------------------------------------------------------------------
void ethernet_fprint(struct eth_header* header, FILE* fd);
//------------------------------------------------------------------------------
//                           ALLOCATION
//------------------------------------------------------------------------------
struct eth_header* to_ethernet_header(char* data);
struct eth_header* ethernet_header_alloc(uint32_t payload_size);
//------------------------------------------------------------------------------
//                           SETTER
//------------------------------------------------------------------------------
void ethernet_header_set(struct eth_header* header, uint16_t type,
                         char* payload, uint32_t len, unsigned char* src_mac,
                         unsigned char* dst_mac);
//------------------------------------------------------------------------------
//                            GETTERS
//------------------------------------------------------------------------------
unsigned char* ethernet_get_src_mac(struct eth_header* header);
unsigned char* ethernet_get_dst_mac(struct eth_header* header);
unsigned char* ethernet_get_payload(struct eth_header* header);
uint16_t ethernet_get_type(struct eth_header* header);

#endif

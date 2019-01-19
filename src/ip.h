#ifndef IP_H
#define IP_H
#define PROTOCOL_ICMP 1
#define PROTOCOL_TCP 6
#define PROTOCOL_UDP 16

struct ip_header;
//------------------------------------------------------------------------------
//                                  ALLOCATION
//------------------------------------------------------------------------------
struct ip_header* ip_header_alloc(uint16_t len, uint8_t protocol, uint32_t src,
                                  uint32_t dst, unsigned char** data);
//------------------------------------------------------------------------------
//                                    GETTERS
//------------------------------------------------------------------------------
uint8_t ip_header_get_protocol(struct ip_header* header);
uint16_t ip_header_get_payload_length(struct ip_header* header);
uint8_t ip_header_get_ttl(struct ip_header* header);
uint32_t ip_header_get_src_addr(struct ip_header* header);
uint32_t ip_header_get_dst_addr(struct ip_header* header);
unsigned char* ip_header_get_payload(struct ip_header* header);
//------------------------------------------------------------------------------
//                             SETTER
//------------------------------------------------------------------------------
void ip_header_set_ttl(struct ip_header* header, uint8_t ttl);
void ip_header_set_dst(struct ip_header* header, uint32_t dst);
void ip_header_set_src(struct ip_header* header, uint32_t src);
void ip_header_set_payload_length(struct ip_header* header, uint32_t len);
void ip_header_set_protocol(struct ip_header* header, uint8_t protocol);
void ip_header_update_checksum(struct ip_header* header);
//------------------------------------------------------------------------------
//                              PRINT
//------------------------------------------------------------------------------
void ip_header_fprint(FILE* fd, struct ip_header* header);
//------------------------------------------------------------------------------
//                              RECEPTION
//------------------------------------------------------------------------------
void ip_receive(struct net_interface* interface,
                struct eth_header* eth_header);
//------------------------------------------------------------------------------
//                              SEND
//------------------------------------------------------------------------------
void ip_send_packet(struct net_interface* interface, struct ip_header* header);
void ip_send(struct net_interface* interface, uint32_t dst, char* payload,
             uint32_t len, uint32_t protocol);

#endif

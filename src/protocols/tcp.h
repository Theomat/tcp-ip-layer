#ifndef TCP_H
#define TCP_H
#define TCP_CONTROL_FIN 1
#define TCP_CONTROL_SYN 32
#define TCP_CONTROL_RST 4
#define TCP_CONTROL_PSH 8
#define TCP_CONTROL_ACK 16
#define TCP_CONTROL_URG 2

struct tcp_header;
//------------------------------------------------------------------------------
//                              LIFECYCLE
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                              PRINT
//------------------------------------------------------------------------------
void tcp_header_fprint(FILE* fd, struct tcp_header* header);
//------------------------------------------------------------------------------
//                                    GETTERS
//------------------------------------------------------------------------------
uint32_t tcp_header_get_sequence(struct tcp_header* header);
uint32_t tcp_header_get_ack(struct tcp_header* header);
uint8_t tcp_header_get_data_offset(struct tcp_header* header);
uint8_t tcp_header_get_control_bits(struct tcp_header* header);
uint16_t tcp_header_get_window(struct tcp_header* header);
uint16_t tcp_header_get_src_port(struct tcp_header* header);
uint16_t tcp_header_get_dst_port(struct tcp_header* header);
unsigned char* tcp_header_get_payload(struct tcp_header* header);
//------------------------------------------------------------------------------
//                              RECEPTION
//------------------------------------------------------------------------------
void tcp_receive(struct net_interface* interface, struct ip_header* ip_header);

#endif

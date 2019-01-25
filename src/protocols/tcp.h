#include <stdint.h>
#include <stdio.h>

#include "ip.h"

#ifndef TCP_H
#define TCP_H
#define TCP_CONTROL_FIN 1
#define TCP_CONTROL_SYN 32
#define TCP_CONTROL_RST 4
#define TCP_CONTROL_PSH 8
#define TCP_CONTROL_ACK 16
#define TCP_CONTROL_URG 2

struct tcp_header {
  uint16_t src_port;
  uint16_t dst_port;
  uint32_t seq_num;
  uint32_t ack_num;
  // Data Offset (4bits) Reserved(6bits) Letters(6bits)
  uint16_t reserved;
  uint16_t window;
  uint16_t checksum;
  uint16_t urgent_ptr;
  uint32_t options_and_padding;
} __attribute__((packed));
//------------------------------------------------------------------------------
//                              LIFECYCLE
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                              PRINT
//------------------------------------------------------------------------------
void tcp_header_fprint(FILE* fd, const struct tcp_header* header);
//------------------------------------------------------------------------------
//                                    GETTERS
//------------------------------------------------------------------------------
#define tcp_header_get_data_offset(header) ((header->reserved >> 12) & 0xf)
#define tcp_header_get_control_bits(header) (header->reserved & 63)
#define tcp_header_payload(header)                                             \
  ((unsigned char*)header + sizeof(struct tcp_header))
//------------------------------------------------------------------------------
//                              RECEPTION
//------------------------------------------------------------------------------
void tcp_receive(struct ip_header* ip_header);

#endif

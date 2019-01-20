#include <arpa/inet.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1

#include "../ethernet.h"
#include "../net_interface.h"

#include "../utils/checksum.h"
#include "../utils/log.h"
#include "../utils/print_utils.h"
#include "ip.h"

#include "tcp.h"

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
void tcp_header_fprint(FILE* fd, struct tcp_header* header) {
  assert(header != NULL);
  fprintf(fd,
          "TCP header [src_port=%u, dst_port=%u, seq_num=%u, ack_num=%u, "
          "data_offset=%u, reserved=%u, control_bits=%u, window=%u, "
          "checksum=%u, urgent_ptr=%u, options=%u]",
          header->src_port, header->dst_port, header->seq_num, header->ack_num,
          ((header->reserved >> 12) & 0xf), ((header->reserved >> 6) & 63),
          (header->reserved & 63), header->window, header->checksum,
          header->urgent_ptr, (header->options_and_padding >> 8));
}
//------------------------------------------------------------------------------
//                                    GETTERS
//------------------------------------------------------------------------------
uint32_t tcp_header_get_sequence(struct tcp_header* header) {
  assert(header != NULL);
  return header->seq_num;
}
uint32_t tcp_header_get_ack(struct tcp_header* header) {
  assert(header != NULL);
  return header->ack_num;
}
uint8_t tcp_header_get_data_offset(struct tcp_header* header) {
  assert(header != NULL);
  return ((header->reserved >> 12) & 0xf);
}
uint8_t tcp_header_get_control_bits(struct tcp_header* header) {
  assert(header != NULL);
  return (header->reserved & 63);
}
uint16_t tcp_header_get_window(struct tcp_header* header) {
  assert(header != NULL);
  return header->window;
}
uint16_t tcp_header_get_src_port(struct tcp_header* header) {
  assert(header != NULL);
  return header->src_port;
}
uint16_t tcp_header_get_dst_port(struct tcp_header* header) {
  assert(header != NULL);
  return header->dst_port;
}
unsigned char* tcp_header_get_payload(struct tcp_header* header) {
  assert(header != NULL);
  return (unsigned char*)header + sizeof(struct tcp_header);
}
//------------------------------------------------------------------------------
//                         CHECK
//------------------------------------------------------------------------------
static bool tcp_header_check(struct ip_header* ip_header,
                             struct tcp_header* header) {
  assert(header != NULL);
  uint32_t len = ip_header_get_payload_length(ip_header);
  uint32_t sum = ~internet_checksum((char*)header, len) + htons(len) +
                 htons(PROTOCOL_TCP) +
                 htonl(ip_header_get_src_addr(ip_header)) +
                 htonl(ip_header_get_dst_addr(ip_header));
  while (sum >> 16)
    sum = (sum & 0xffff) + (sum >> 16);

  return (uint16_t)(sum + 2) == 0;
}
//------------------------------------------------------------------------------
//                              RECEPTION
//------------------------------------------------------------------------------
void tcp_receive(struct net_interface* interface, struct ip_header* ip_header) {
  assert(interface != NULL);
  assert(ip_header != NULL);

  struct tcp_header* header =
      (struct tcp_header*)ip_header_get_payload(ip_header);

  if (!tcp_header_check(ip_header, header)) {
    ERROR("TCP: Invalid checksum for packet : ");
    tcp_header_fprint(stderr, header);
    fprintf(stderr, "\n");
    return;
  }

  header->seq_num  = ntohs(header->seq_num);
  header->ack_num  = ntohs(header->ack_num);
  header->dst_port = ntohs(header->dst_port);
  header->src_port = ntohs(header->src_port);

#if DEBUG
  LOG_DEBUG("[TCP] Received ");
  tcp_header_fprint(stdout, header);
  printf("\n");
#endif

  if (tcp_header_get_control_bits(header) == TCP_CONTROL_SYN) {
    LOG_DEBUG("[TCP] SYN attempt recorded.\n");
  }
}

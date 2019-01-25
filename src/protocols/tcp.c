#include "tcp.h"
#include <arpa/inet.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#define DEBUG 1

#include "../net_interface.h"
#include "../utils/log.h"
#include "../utils/protocols.h"

//------------------------------------------------------------------------------
//                              LIFECYCLE
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                              PRINT
//------------------------------------------------------------------------------
void tcp_header_fprint(FILE* fd, const struct tcp_header* header) {
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
//                         CHECK
//------------------------------------------------------------------------------
static bool tcp_header_check(struct ip_header* ip_header,
                             struct tcp_header* header) {
  assert(header != NULL);
  uint32_t len = ip_header_get_payload_length(ip_header);
  uint32_t sum = ~internet_checksum((char*)header, len) + htons(len) +
                 htons(PROTOCOL_TCP) + htonl(ip_header->src_addr) +
                 htonl(ip_header->dst_addr);
  while (sum >> 16)
    sum = (sum & 0xffff) + (sum >> 16);

  return (uint16_t)(sum + 2) == 0;
}

void tcp_header_ntoh(struct tcp_header* header) {
  header->seq_num  = ntohs(header->seq_num);
  header->ack_num  = ntohs(header->ack_num);
  header->dst_port = ntohs(header->dst_port);
  header->src_port = ntohs(header->src_port);
}
void tcp_header_hton(struct tcp_header* header) {
  header->seq_num  = htons(header->seq_num);
  header->ack_num  = htons(header->ack_num);
  header->dst_port = htons(header->dst_port);
  header->src_port = htons(header->src_port);
}
//------------------------------------------------------------------------------
//                              RECEPTION
//------------------------------------------------------------------------------
void tcp_receive(struct ip_header* ip_header) {
  assert(ip_header != NULL);

  struct tcp_header* header = (struct tcp_header*)ip_header->payload;

  if (!tcp_header_check(ip_header, header)) {
    ERROR("TCP: Invalid checksum for packet : ");
    tcp_header_fprint(stderr, header);
    fprintf(stderr, "\n");
    return;
  }

  tcp_header_ntoh(header);

#if DEBUG
  LOG_DEBUG("[TCP] Received ");
  tcp_header_fprint(stdout, header);
  printf("\n");
#endif

  if (tcp_header_get_control_bits(header) == TCP_CONTROL_SYN) {
    LOG_DEBUG("[TCP] SYN attempt recorded.\n");
  }
}

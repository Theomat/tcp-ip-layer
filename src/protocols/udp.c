#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1

#include "../net_interface.h"
#include "../utils/log.h"
#include "../utils/print_utils.h"
#include "../utils/protocols.h"
#include "ip.h"

#include "udp.h"

struct udp_header {
  uint16_t src_port;
  uint16_t dst_port;
  uint16_t len;
  uint16_t checksum;
} __attribute__((packed));
//------------------------------------------------------------------------------
//                              PRINT
//------------------------------------------------------------------------------
void udp_header_fprint(FILE* fd, const struct udp_header* header) {
  assert(header != NULL);
  fprintf(fd, "UDP header [src_port=%u, dst_port=%u, len=%lu(%u), checksum=%u]",
          header->src_port, header->dst_port,
          header->len - sizeof(struct udp_header), header->len,
          header->checksum);
}
//------------------------------------------------------------------------------
//                              RECEPTION
//------------------------------------------------------------------------------
void udp_receive(struct ip_header* ip_header) {
  assert(ip_header != NULL);

  struct udp_header* header = (struct udp_header*)ip_header->payload;
#if DEBUG
  LOG_DEBUG("[UDP] Received ");
  udp_header_fprint(stdout, header);
  printf("\n");
#endif
}

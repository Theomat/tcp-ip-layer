#include <arpa/inet.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ethernet.h"
#include "../net_interface.h"

#define DEBUG 0

#include "../utils/checksum.h"
#include "../utils/log.h"
#include "ip.h"

#include "icmp.h"
//------------------------------------------------------------------------------
//                            STRUCTURES
//------------------------------------------------------------------------------
struct icmp_v4 {
  // 0 (Echo Reply), 3 (Destination Unreachable) and 8 (Echo request)
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
  uint8_t data[];
} __attribute__((packed));

struct icmp_v4_echo {
  uint16_t id;
  uint16_t seq;
  uint8_t data[];
} __attribute__((packed));

struct icmp_v4_dst_unreachable {
  uint8_t unused;
  uint8_t len;
  uint16_t var;
  uint8_t data[];
} __attribute__((packed));

#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO_REQUEST 8
#define ICMP_DESTINATION_UNREACHABLE 3
//------------------------------------------------------------------------------
//                              PRINT
//------------------------------------------------------------------------------
void icmp_fprint(FILE* fd, struct icmp_v4* header, uint32_t len) {
  assert(header != NULL);
  fprintf(fd, "ICMPv4 header [type=%u, code=%u, checksum=%u, length=%lu(%u)]",
          header->type, header->code, header->checksum,
          len - sizeof(struct icmp_v4), len);
}
//------------------------------------------------------------------------------
//                              CHECK
//------------------------------------------------------------------------------
static bool icmp_check(struct icmp_v4* header, uint32_t len) {
  return 0 == internet_checksum((char*)header, len);
}
//------------------------------------------------------------------------------
//                            ECHO REPLY
//------------------------------------------------------------------------------
static void receive_echo_request(struct net_interface* interface,
                                 struct ip_header* ip_header,
                                 struct icmp_v4* header) {
  assert(interface != NULL);
  assert(ip_header != NULL);
  assert(header != NULL);

  uint32_t len = ip_header_get_payload_length(ip_header);

  header->type     = ICMP_ECHO_REPLY;
  header->checksum = 0;
  header->checksum = internet_checksum((char*)header, len);
#if DEBUG
  LOG_DEBUG("[ICMPv4] Sent ");
  icmp_fprint(stdout, header, len);

  struct icmp_v4_echo* echo = (struct icmp_v4_echo*)header->data;
  printf(" ECHO (id=%u, seq=%u, length=%lu(%lu))", ntohs(echo->id),
         ntohs(echo->seq),
         len - sizeof(struct icmp_v4) - sizeof(struct icmp_v4_echo),
         len - sizeof(struct icmp_v4));

  printf("\n");
#endif

  ip_header_set_dst(ip_header, ip_header_get_src_addr(ip_header));
  ip_header_set_src(ip_header, ntohl(net_interface_get_ip(interface)));

  ip_send_packet(interface, ip_header);
}
//------------------------------------------------------------------------------
//                            ICMP RECEPTION
//------------------------------------------------------------------------------
void icmp_receive(struct net_interface* interface,
                  struct ip_header* ip_header) {
  assert(interface != NULL);
  assert(ip_header != NULL);
  struct icmp_v4* header = (struct icmp_v4*)ip_header_get_payload(ip_header);
  if (!icmp_check(header, ip_header_get_payload_length(ip_header))) {
    ERROR("ICMPv4: Invalid packet checksum\n");
    return;
  }

#if DEBUG
  LOG_DEBUG("[ICMPv4] Received ");
  icmp_fprint(stdout, header, ip_header_get_payload_length(ip_header));
  printf("\n");
#endif

  switch (header->type) {
  case ICMP_ECHO_REPLY:
    ERROR("ICMPv4: Unsupported message type: ECHO reply\n");
    break;
  case ICMP_ECHO_REQUEST:
    LOG_DEBUG("[ICMPv4] Received ECHO request.\n");
    receive_echo_request(interface, ip_header, header);
    break;
  case ICMP_DESTINATION_UNREACHABLE:
    ERROR("ICMPv4: Unsupported message type: destination unreachable\n");
    break;
  default:
    ERROR("ICMPv4: Unsupported message type %u\n", header->type);
    break;
  }
}

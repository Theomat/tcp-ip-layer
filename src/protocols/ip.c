#include "ip.h"
#include <arpa/inet.h>
#include <assert.h>
#include <linux/if_ether.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0

#include "../net_interface.h"
#include "../utils/log.h"
#include "../utils/print_utils.h"
#include "../utils/protocols.h"
#include "arp.h"
#include "icmp.h"
#include "tcp.h"
#include "udp.h"

//------------------------------------------------------------------------------
//                                  ALLOCATION
//------------------------------------------------------------------------------
static struct ip_header* ip_header_alloc(uint16_t len) {
  struct ip_header* header          = calloc(1, sizeof(struct ip_header) + len);
  header->version_and_header_len    = (4 << 4) + 5;
  header->service_type              = 0;
  header->len                       = sizeof(struct ip_header) + len;
  header->index                     = 7; // RFC 791 show an id of 111 == 7
  header->flags_and_fragment_offset = 0;
  header->ttl                       = 64;
  header->protocol                  = 0;
  header->checksum                  = 0;
  header->src_addr                  = 0;
  header->dst_addr                  = 0;
  header->checksum = internet_checksum((char*)header, sizeof(struct ip_header));
  return header;
}
//------------------------------------------------------------------------------
//                         CHECK
//------------------------------------------------------------------------------
void ip_header_update_checksum(struct ip_header* header) {
  assert(header != NULL);
  header->checksum = 0;
  header->checksum = internet_checksum((char*)header, sizeof(struct ip_header));
}
static bool ip_header_check(const struct ip_header* header) {
  assert(header != NULL);
  return 0 == internet_checksum((char*)header, sizeof(struct ip_header));
}
//------------------------------------------------------------------------------
//                              PRINT
//------------------------------------------------------------------------------
void ip_header_fprint(FILE* fd, const struct ip_header* header) {
  assert(header != NULL);
  fprintf(fd,
          "IP header [version=%u, header_length=%u, tos=%u, length=%lu(%u), "
          "index=%u, flags=%u, fragment_offset=%u, ttl=%u, protocol=%u, "
          "checksum=%u, src=",
          ip_header_get_version(header), ip_header_get_header_length(header),
          header->service_type, header->len - sizeof(struct ip_header),
          header->len, header->index,
          (header->flags_and_fragment_offset >> 12) & 0xf,
          (header->flags_and_fragment_offset & 0xfff), header->ttl,
          header->protocol, header->checksum);
  ipv4_fprint(fd, header->src_addr);
  fprintf(fd, ", dst=");
  ipv4_fprint(fd, header->dst_addr);
  fprintf(fd, "]");
}
//------------------------------------------------------------------------------
//                              CONVERTION
//------------------------------------------------------------------------------
static void ip_header_ntoh(struct ip_header* header) {
  header->src_addr = ntohl(header->src_addr);
  header->dst_addr = ntohl(header->dst_addr);
  header->len      = ntohs(header->len);
}
static void ip_header_hton(struct ip_header* header) {
  header->src_addr = htonl(header->src_addr);
  header->dst_addr = htonl(header->dst_addr);
  header->len      = htons(header->len);
}
//------------------------------------------------------------------------------
//                              RECEPTION
//------------------------------------------------------------------------------
void ip_receive(struct eth_header* eth_header) {
  assert(eth_header != NULL);
  struct ip_header* header = (struct ip_header*)eth_header->payload;
  // Check for valid checksum
  if (!ip_header_check(header)) {
    ERROR("IPv4: invalid checksum: %u %u\n", header->checksum,
          internet_checksum((char*)header, sizeof(struct ip_header)));
    return;
  }

  ip_header_ntoh(header);

#if DEBUG
  printf("[DEBUG] [IPv4] Received ");
  ip_header_fprint(stdout, header);
  printf("\n");
#endif

  switch (header->protocol) {
  case PROTOCOL_TCP:
    LOG_DEBUG("[IPv4] Received TCP packet.\n");
    tcp_receive(header);
    break;
  case PROTOCOL_UDP:
    LOG_DEBUG("[IPv4] Received UDP packet.\n");
    udp_receive(header);
    break;
  case PROTOCOL_ICMP:
    LOG_DEBUG("[IPv4] Received ICMP packet.\n");
    icmp_receive(header);
    break;
  default:
    ERROR("IPv4: Unsupported protocol : %u\n", header->protocol);
    break;
  }
}
//------------------------------------------------------------------------------
//                              SEND
//------------------------------------------------------------------------------
void ip_send_packet(struct ip_header* header) {
  assert(header != NULL);
  // TODO make a kind of queue to wait for the ip to be resolved before sending
  // data
  uint32_t net_dst_addr  = htonl(header->dst_addr);
  uint32_t real_len      = header->len;
  unsigned char* dst_mac = arp_resolve_ipv4(net_dst_addr);
  if (dst_mac == NULL) {
    ERROR("IPv4: Could not resolve MAC address for ");
    ipv4_fprint(stderr, header->dst_addr);
    printf("\n");
    exit(1);
  }

#if DEBUG
  printf("[DEBUG] [IPv4] Sent ");
  ip_header_fprint(stdout, header);
  printf("\n");
#endif

  ip_header_hton(header);
  ip_header_update_checksum(header);

  net_interface_send((char*)header, real_len, dst_mac, ETH_P_IP);
}
void ip_send(uint32_t dst, char* payload, uint32_t len, uint32_t protocol) {
  assert(payload != NULL);
  struct ip_header* ip_header = ip_header_alloc(len);
  ip_header->protocol         = protocol;
  ip_header->src_addr         = ntohl(net_interface_get_ip());
  ip_header->dst_addr         = dst;

  memcpy(ip_header->payload, payload, len);

  ip_send_packet(ip_header);
  free(ip_header);
}

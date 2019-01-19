#include <arpa/inet.h>
#include <assert.h>
#include <linux/if_ether.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ethernet.h"
#include "../net_interface.h"

#include "ip.h"

#define DEBUG 0

#include "../utils/checksum.h"
#include "../utils/log.h"
#include "../utils/print_utils.h"
#include "arp.h"
#include "icmp.h"

#define PROTOCOL_ICMP 1
#define PROTOCOL_TCP 6
#define PROTOCOL_UDP 16

struct ip_header {
  // Protocol version : 4 for IPv4 (1st four bytes)
  uint8_t version_and_header_len;
  // (2nd four bytes) Internet Header Length == number of 32-bit words in header
  // : 4 for IPv4

  // Type Of Service : (from 1st specification)
  uint8_t service_type;
  // Length of the whole IP datagram
  uint16_t len;
  // Used in fragmentation
  uint16_t index;
  // Used in fragmentation
  uint16_t flags_and_fragment_offset;
  // Position of a fragment in a IP datagram
  // Time to Live
  uint8_t ttl;
  // Protocol Type in payload: UDP 16, TCP 6, ICMP 1
  uint8_t protocol;
  // Checksum
  uint16_t checksum;
  uint32_t src_addr;
  uint32_t dst_addr;
} __attribute__((packed));
//------------------------------------------------------------------------------
//                                  ALLOCATION
//------------------------------------------------------------------------------
struct ip_header* ip_header_alloc(uint16_t len, uint8_t protocol, uint32_t src,
                                  uint32_t dst, unsigned char** data) {
  assert(data != NULL);
  struct ip_header* header          = calloc(1, sizeof(struct ip_header) + len);
  header->version_and_header_len    = (4 << 4) + 5;
  header->service_type              = 0;
  header->len                       = sizeof(struct ip_header) + len;
  header->index                     = 7; // RFC 791 show an id of 111 == 7
  header->flags_and_fragment_offset = 0;
  header->ttl                       = 64;
  header->protocol                  = protocol;
  header->checksum                  = 0;
  header->src_addr                  = src;
  header->dst_addr                  = dst;
  header->checksum = internet_checksum((char*)header, sizeof(struct ip_header));
  *data            = ((unsigned char*)header) + sizeof(struct ip_header);
  return header;
}
//------------------------------------------------------------------------------
//                                    GETTERS
//------------------------------------------------------------------------------
uint8_t ip_header_get_protocol(struct ip_header* header) {
  assert(header != NULL);
  return header->protocol;
}

uint16_t ip_header_get_payload_length(struct ip_header* header) {
  assert(header != NULL);
  return header->len - sizeof(struct ip_header);
}
uint8_t ip_header_get_ttl(struct ip_header* header) {
  assert(header != NULL);
  return header->ttl;
}
uint32_t ip_header_get_src_addr(struct ip_header* header) {
  assert(header != NULL);
  return header->src_addr;
}
uint32_t ip_header_get_dst_addr(struct ip_header* header) {
  assert(header != NULL);
  return header->dst_addr;
}
unsigned char* ip_header_get_payload(struct ip_header* header) {
  assert(header != NULL);
  return (unsigned char*)header + sizeof(struct ip_header);
}
//------------------------------------------------------------------------------
//                             SETTER
//------------------------------------------------------------------------------
void ip_header_set_ttl(struct ip_header* header, uint8_t ttl) {
  assert(header != NULL);
  header->ttl = ttl;
}
void ip_header_set_dst(struct ip_header* header, uint32_t dst) {
  assert(header != NULL);
  header->dst_addr = dst;
}
void ip_header_set_src(struct ip_header* header, uint32_t src) {
  assert(header != NULL);
  header->src_addr = src;
}
void ip_header_set_payload_length(struct ip_header* header, uint32_t len) {
  assert(header != NULL);
  header->len = len + sizeof(struct ip_header);
}
void ip_header_set_protocol(struct ip_header* header, uint8_t protocol) {
  assert(header != NULL);
  header->protocol = protocol;
}
void ip_header_update_checksum(struct ip_header* header) {
  assert(header != NULL);
  header->checksum = 0;
  header->checksum = internet_checksum((char*)header, sizeof(struct ip_header));
}

//------------------------------------------------------------------------------
//                         CHECK
//------------------------------------------------------------------------------
static bool ip_header_check(struct ip_header* header) {
  assert(header != NULL);
  return 0 == internet_checksum((char*)header, sizeof(struct ip_header));
}
//------------------------------------------------------------------------------
//                              PRINT
//------------------------------------------------------------------------------
void ip_header_fprint(FILE* fd, struct ip_header* header) {
  assert(header != NULL);
  fprintf(fd,
          "IP header [version=%u, header_length=%u, tos=%u, length=%lu(%u), "
          "index=%u, flags=%u, fragment_offset=%u, ttl=%u, protocol=%u, "
          "checksum=%u, src=",
          (header->version_and_header_len >> 4),
          header->version_and_header_len & 0xf, header->service_type,
          header->len - sizeof(struct ip_header), header->len, header->index,
          (header->flags_and_fragment_offset >> 12) & 0xf,
          (header->flags_and_fragment_offset & 0xfff), header->ttl,
          header->protocol, header->checksum);
  ipv4_fprint(fd, header->src_addr);
  fprintf(fd, ", dst=");
  ipv4_fprint(fd, header->dst_addr);
  fprintf(fd, "]");
}
//------------------------------------------------------------------------------
//                              RECEPTION
//------------------------------------------------------------------------------
void ip_receive(struct net_interface* interface,
                struct eth_header* eth_header) {
  assert(interface != NULL);
  assert(eth_header != NULL);
  struct ip_header* header =
      (struct ip_header*)ethernet_get_payload(eth_header);
  // Check for valid checksum
  if (!ip_header_check(header)) {
    ERROR("IPv4: invalid checksum: %u %u\n", header->checksum,
          internet_checksum((char*)header, sizeof(struct ip_header)));
    return;
  }

  header->len = ntohs(header->len);

#if DEBUG
  printf("[DEBUG] [IPv4] Received ");
  ip_header_fprint(stdout, header);
  printf("\n");
#endif

  switch (ip_header_get_protocol(header)) {
  case PROTOCOL_TCP:
    ERROR("IPv4: Unsupported protocol TCP\n");
    break;
  case PROTOCOL_UDP:
    ERROR("IPv4: Unsupported protocol UDP\n");
    break;
  case PROTOCOL_ICMP:
    LOG_DEBUG("[IPv4] Received ICMP packet.\n");
    icmp_receive(interface, header);
    break;
  default:
    ERROR("IPv4: Unsupported protocol : %u\n", ip_header_get_protocol(header));
    break;
  }
}
//------------------------------------------------------------------------------
//                              SEND
//------------------------------------------------------------------------------
void ip_send_packet(struct net_interface* interface, struct ip_header* header) {
  assert(interface != NULL);
  // TODO make a kind of queue to wait for the ip to be resolved before sending
  // data
  uint32_t real_len      = header->len;
  unsigned char* dst_mac = arp_resolve_ipv4(interface, header->dst_addr);
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

  header->len = htons(header->len);
  ip_header_update_checksum(header);

  net_interface_send(interface, (char*)header, real_len, dst_mac, ETH_P_IP);
}
void ip_send(struct net_interface* interface, uint32_t dst, char* payload,
             uint32_t len, uint32_t protocol) {
  assert(interface != NULL);
  unsigned char** payload_dst = calloc(1, sizeof(unsigned char**));
  struct ip_header* ip_header = ip_header_alloc(
      len, protocol, net_interface_get_ip(interface), dst, payload_dst);

  memcpy(*payload_dst, payload, len);
  free(payload_dst);

  ip_send_packet(interface, ip_header);
  free(ip_header);
}

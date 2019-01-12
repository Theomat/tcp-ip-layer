#include <arpa/inet.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ethernet.h"

struct eth_header {
  unsigned char dst_mac[6];
  unsigned char src_mac[6];
  uint16_t ether_type;
  // If ether_type >= 1536 : ether_type == payload type IPv4 or ARP
  // Else : ether_type == payload_length
  unsigned char payload[];
  // We discard frame tags & CRC check
} __attribute__((packed));

void ethernet_fprint(struct eth_header* header, FILE* fd) {
  assert(header != NULL);
  fprintf(fd,
          "ETH header [dst=%u:%u:%u:%u:%u:%u, src=%u:%u:%u:%u:%u:%u, type=%u]",
          header->dst_mac[0], header->dst_mac[1], header->dst_mac[2],
          header->dst_mac[3], header->dst_mac[4], header->dst_mac[5],
          header->src_mac[0], header->src_mac[1], header->src_mac[2],
          header->src_mac[3], header->src_mac[4], header->src_mac[5],
          header->ether_type);
}

struct eth_header* to_ethernet_header(char* data) {
  struct eth_header* header = ((struct eth_header*)data);
  header->ether_type        = htons(header->ether_type);
  return header;
}

struct eth_header* ethernet_header_alloc() {
  return calloc(1, sizeof(struct eth_header) + sizeof(char) * 100);
}

void ethernet_header_set(struct eth_header* header, uint16_t type,
                         char* payload, uint32_t len, unsigned char* src_mac,
                         unsigned char* dst_mac) {
  assert(header != NULL);
  header->ether_type = htons(type);
  memcpy(header->dst_mac, dst_mac, 6);
  memcpy(header->src_mac, src_mac, 6);
  memcpy(header->payload, payload, len);
  header->payload[len] = 0;
}

unsigned char* ethernet_payload(struct eth_header* header) {
  assert(header != NULL);
  return header->payload;
}

uint16_t ethernet_type(struct eth_header* header) {
  assert(header != NULL);
  return header->ether_type;
}

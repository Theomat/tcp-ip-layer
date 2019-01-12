#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ethernet.h"
#include "hashmap.h"
#include "net_interface.h"

#include "arp.h"

#define ARP_ETHERNET 0x0001
#define ARP_IPV4 0x0800
#define ARP_REQUEST 0x0001
#define ARP_REPLY 0x0002

struct arp_header {
  // link layer type used : Ethernet 0x0001
  uint16_t hw_type;
  // protocol type : IPv4 0x0800
  uint16_t pro_type;
  // MAC address size : 6Bytes | IPv4 address size : 4Bytes
  unsigned char hw_size;
  unsigned char pro_size;
  // Request : ARP request(1), ARP reply(2), RARP request(3), RARP reply(4)
  uint16_t opcode;
  unsigned char data[];
} __attribute__((packed));

struct arp_ipv4 {
  unsigned char src_mac[6];
  uint32_t src_ip;
  unsigned char dst_mac[6];
  uint32_t dst_ip;
} __attribute__((packed));

// Currently we discard the protocol type from the transition table as there's
// only one protocol type supported for now
static struct hashmap* transition_table = NULL;
static unsigned long hash(const void* src_ip) {
  unsigned long u = *((uint32_t*)src_ip);
  return u;
}
static long equals(const void* ip_a, const void* ip_b) {
  uint32_t* a = ((uint32_t*)ip_a);
  uint32_t* b = ((uint32_t*)ip_b);
  return a - b;
}

void arp_init() { transition_table = hashmap_alloc(16, &hash, &equals); }
void arp_destroy() {
  struct hashmap_iterator* it = hashmap_it_alloc();
  hashmap_it_set_map(transition_table);
  void** key  = &(&transition_table);
  void** cell = &(&transition_table);
  while (hashmap_it_has_next(it)) {
    hashmap_it_next(it, key, cell);
    free(key);
    free(cell);
  }
  free(it);
  free_hashmap(transition_table);
}
static bool is_in_transition_table(uint16_t pro_type, uint32_t src_ip) {
  return hashmap_contains_key(transition_table, &src_ip);
}
static char* alloc_src_mac() { return calloc(6, sizeof(char)); }
static void set_in_transition_table(uint16_t pro_type, uint32_t src_ip,
                                    char* src_mac) {
  char* dst_mac =
      hashmap_lget_or_default(transition_table, src_ip, &alloc_src_mac);
  memcpy(dst_mac, src_mac, 6);
  hashmap_put(transition_table, &src_ip, &dst_mac);
}
static void add_to_transition_table(uint16_t pro_type, uint32_t src_ip,
                                    char* src_mac) {
  uint32_t* dst_ip = calloc(1, sizeof(uint32_t));
  *dst_ip          = src_ip;
  char* dst_mac    = alloc_src_mac();
  memcpy(dst_mac, src_mac, 6);
  hashmap_put(transition_table, &dst_ip, &dst_mac);
}

static void arp_reply(struct net_interface* interface,
                      struct arp_header* header) {
  struct arp_ipv4* content = (struct arp_ipv4*)header->data;
  content->dst_ip          = content->src_ip;
  memcpy(content->dst_mac, content->src_map, 6);
  content->src_ip = net_interface_get_ip(interface);
  memcpy(content->src_mac, net_interface_get_mac(interface), 6);

  header->opcode = ARP_REPLY;

  // TODO : Send the packet to the (new) target hardware address on the
  // same hardware on which the request was received.
}

void arp_resolution(struct net_interface* interface,
                    struct eth_header* eth_header) {
  assert(interface != NULL);
  assert(eth_header != NULL);
  struct arp_header* header = (struct arp_header*)ethernet_payload(eth_header);
  // Check hardware type
  if (header->hw_type == ARP_ETHERNET) {
    // Check protocol type
    if (header->pro_type == ARP_IPV4) {
      bool merge_flag          = false;
      struct arp_ipv4* content = (struct arp_ipv4*)header->data;
      if (is_in_transition_table(header->pro_type, content->src_ip)) {
        set_in_transition_table(header->pro_type, content->src_ip,
                                content->src_mac);
        merge_flag = true;
      }
      // Check we are the target
      if (content->dst_ip == net_interface_get_ip(interface)) {
        if (!merge_flag) {
          add_to_transition_table(header->pro_type, content->src_ip,
                                  content->src_mac);
        }
        if (header->opcode == ARP_REQUEST) {
          arp_reply(interface, header);
          // Normal exit -> no error
          return;
        }
      } else {
        // Normal exit -> no error when we aren't the target
        return;
      }
    }
  }
  fprintf(stderr, "ARP:(arp_resolution) Unsupported data\n");
}

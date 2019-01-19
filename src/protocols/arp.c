#include <arpa/inet.h>
#include <assert.h>
#include <linux/if_ether.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0

#include "../collections/hashmap.h"
#include "../ethernet.h"
#include "../net_interface.h"
#include "../utils/log.h"
#include "../utils/print_utils.h"

#include "arp.h"
//------------------------------------------------------------------------------
//                                  STRUCTURES
//------------------------------------------------------------------------------
#define ARP_ETHERNET 1
#define ARP_IPV4 0x0800
#define ARP_REQUEST 1
#define ARP_REPLY 2

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
//------------------------------------------------------------------------------
//                                  PRINT
//------------------------------------------------------------------------------
void arp_header_fprint(struct arp_header* header, FILE* fd) {
  assert(header != NULL);
  fprintf(fd,
          "ARP header [hw=%u, hw_size=%u, protocol=%u, protocol_size=%u, "
          "opcode=%u]",
          header->hw_type, header->hw_size, header->pro_type, header->pro_size,
          header->opcode);
}
//------------------------------------------------------------------------------
//                           TRANSITION TABLE
//------------------------------------------------------------------------------
#define TRANSITION_TABLE_INITIAL_SIZE 16
static struct hashmap* transition_table = NULL;
// The hash should be improved, because pro_type has little to no impact on the
// hash itself
static unsigned long hash(const void* ptr) { return *((uint64_t*)ptr); }
static long equals(const void* ptr_a, const void* ptr_b) {
  uint64_t* a = ((uint64_t*)ptr_a);
  uint64_t* b = ((uint64_t*)ptr_b);
  return *a == *b;
}
static uint64_t to_key(uint16_t pro_type, uint32_t src_ip) {
  return (((uint64_t)pro_type << 32)) + src_ip;
}

static bool is_in_transition_table(uint16_t pro_type, uint32_t src_ip) {
  uint64_t key = to_key(pro_type, src_ip);
  return hashmap_contains_key(transition_table, &key);
}
// Can be called only if (pro_type, src_ip) is in the transition table
static void set_in_transition_table(uint16_t pro_type, uint32_t src_ip,
                                    unsigned char* src_mac) {
  unsigned char* dst_mac = NULL;
  uint64_t key           = to_key(pro_type, src_ip);

  dst_mac = hashmap_get(transition_table, &key);
  assert(dst_mac != NULL);
  memcpy(dst_mac, src_mac, 6);
  hashmap_put(transition_table, &key, dst_mac);
}
static void add_to_transition_table(uint16_t pro_type, uint32_t src_ip,
                                    unsigned char* src_mac) {
  uint64_t* dst_key      = calloc(1, sizeof(uint64_t));
  unsigned char* dst_mac = calloc(6, sizeof(char));

  *dst_key = to_key(pro_type, src_ip);
  memcpy(dst_mac, src_mac, 6);
  hashmap_put(transition_table, dst_key, dst_mac);
}
//------------------------------------------------------------------------------
//                              ARP LIFECYCLE
//------------------------------------------------------------------------------
// Allocate the transition table
void arp_init() {
  transition_table =
      hashmap_alloc(TRANSITION_TABLE_INITIAL_SIZE, &hash, &equals);
}
// Free transition table
void arp_destroy() {
  struct hashmap_iterator* it = hashmap_it_alloc();
  // Init them as anything but NULL
  void** key  = calloc(1, sizeof(void**));
  void** cell = calloc(1, sizeof(void**));
  LOG_DEBUG("[ARP] Dumping cache %ld :\n", hashmap_size(transition_table));
  hashmap_it_set_map(it, transition_table);
  while (hashmap_it_has_next(it)) {
    hashmap_it_next(it, key, cell);

#if DEBUG
    uint64_t ip_and_pro = *((uint64_t*)*key);
    LOG_DEBUG("[ARP] [CACHE] IP=");
    ipv4_fprint(stdout,
                ip_and_pro); // Only prints the lowest 32 bits so no issue
    printf(" MAC=");
    mac_fprint(stdout, ((unsigned char*)*cell));
    printf("\n");
#endif
    free(*key);
    free(*cell);
  }
  free(key);
  free(cell);
  free(it);
  free_hashmap(transition_table);
}

//------------------------------------------------------------------------------
//                                ARP REPLY
//------------------------------------------------------------------------------
static void arp_reply(struct net_interface* interface,
                      struct arp_header* header) {
  struct arp_ipv4* content = (struct arp_ipv4*)header->data;
  // Change dest to sender
  content->dst_ip = content->src_ip;
  memcpy(content->dst_mac, content->src_mac, 6);
  // Change src to us
  content->src_ip = net_interface_get_ip(interface);
  memcpy(content->src_mac, net_interface_get_mac(interface), 6);

  header->opcode = ARP_REPLY;
  // Convert local order to network order
  header->opcode   = htons(header->opcode);
  header->hw_type  = htons(header->hw_type);
  header->pro_type = htons(header->pro_type);

  // Send the packet to the (new) target hardware address on the
  // same hardware on which the request was received.
  net_interface_send(interface, (char*)header,
                     sizeof(struct arp_header) + sizeof(struct arp_ipv4),
                     content->dst_mac, ETH_P_ARP);
}
//------------------------------------------------------------------------------
//                             ARP RESOLVE
//------------------------------------------------------------------------------
void arp_receive(struct net_interface* interface,
                 struct eth_header* eth_header) {
  assert(interface != NULL);
  assert(eth_header != NULL);

  struct arp_header* header =
      (struct arp_header*)ethernet_get_payload(eth_header);
  struct arp_ipv4* content = NULL;
  bool merge_flag          = false;

  header->hw_type  = ntohs(header->hw_type);
  header->pro_type = ntohs(header->pro_type);
  header->opcode   = ntohs(header->opcode);

#if DEBUG
  printf("[DEBUG] [ARP] Received ");
  arp_header_fprint(header, stdout);
  printf("\n");
#endif

  // Check hardware type
  if (header->hw_type == ARP_ETHERNET && header->hw_size == 6) {
    // Check protocol type
    if (header->pro_type == ARP_IPV4 && header->pro_size == 4) {

      content = (struct arp_ipv4*)header->data;
      if (is_in_transition_table(header->pro_type, content->src_ip)) {
        set_in_transition_table(header->pro_type, content->src_ip,
                                content->src_mac);
        merge_flag = true;
      }
      // Check we are the target
      if (content->dst_ip == net_interface_get_ip(interface)) {
        if (!merge_flag) {
#if DEBUG
          LOG_DEBUG("[ARP] [CACHE] Added ");
          ipv4_fprint(stdout, content->src_ip);
          printf(" matching MAC ");
          mac_fprint(stdout, content->src_mac);
          printf(".\n");
#endif

          add_to_transition_table(header->pro_type, content->src_ip,
                                  content->src_mac);
        }
        if (header->opcode == ARP_REQUEST) {
          arp_reply(interface, header);
        } else if (header->opcode == ARP_REPLY) {
          // No error we did add the answer in the transition table
          // So all is good
        } else {
          ERROR("ARP: Unsupported opcode %u\n", header->opcode);
        }
      }
    } else {
      ERROR("ARP: Unsupported protocol type %u of size %u\n", header->pro_type,
            header->pro_size);
    }
  } else {
    ERROR("ARP: Unsupported hardware type %u of size %u\n", header->hw_type,
          header->hw_type);
  }
}
// Returns the mac address if is in the arp cache
// If NOT returns NULL and send an ARP request packet to resolve the specific
// ip
unsigned char* arp_resolve_ipv4(struct net_interface* interface,
                                uint32_t ipv4) {
  if (is_in_transition_table(ARP_IPV4, ipv4)) {
    uint64_t key = to_key(ARP_IPV4, ipv4);
    return hashmap_get(transition_table, &key);
  } else {
#if DEBUG
    LOG_DEBUG("[ARP] ");
    ipv4_fprint(stdout, ipv4);
    printf(" could not be resolved directly... Sending packet.\n");
#endif
    // Not in transition table so we should send a broadcast
    struct arp_header* header =
        calloc(1, sizeof(struct arp_header) + sizeof(struct arp_ipv4));
    struct arp_ipv4* content = NULL;
    char broadcast[6]        = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    header->hw_type  = ARP_ETHERNET;
    header->hw_size  = 6;
    header->pro_type = ARP_IPV4;
    header->pro_size = 4;
    header->opcode   = ARP_REQUEST;
    // Convert local order to network order
    header->opcode   = htons(header->opcode);
    header->hw_type  = htons(header->hw_type);
    header->pro_type = htons(header->pro_type);

    content = (struct arp_ipv4*)header->data;
    // Change dest to target to resolve
    content->dst_ip = ipv4;
    // Change mac address to broadcast address
    memcpy(content->dst_mac, broadcast, 6);
    // Change src to us
    content->src_ip = net_interface_get_ip(interface);
    memcpy(content->src_mac, net_interface_get_mac(interface), 6);

    net_interface_broadcast(interface, (char*)header,
                            sizeof(struct arp_header) + sizeof(struct arp_ipv4),
                            ETH_P_ARP);
    free(header);
    return NULL;
  }
}

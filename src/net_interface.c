#include <arpa/inet.h>
#include <assert.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "ethernet.h"
#include "tuntap_interface.h"
#include "utils.h"

#include "net_interface.h"

struct net_interface {
  uint32_t ip;
  unsigned char mac[6];
};
//------------------------------------------------------------------------------
//                               GETTERS
//------------------------------------------------------------------------------
uint32_t net_interface_get_ip(struct net_interface* interface) {
  assert(interface != NULL);
  return interface->ip;
}
unsigned char* net_interface_get_mac(struct net_interface* interface) {
  assert(interface != NULL);
  return interface->mac;
}

static struct eth_header* eth_header = NULL;
static uint32_t max_payload_size     = 0;
//------------------------------------------------------------------------------
//                           SEND DATA
//------------------------------------------------------------------------------
void net_interface_send(struct net_interface* interface, char* content,
                        uint32_t len, unsigned char* dst_mac) {
  assert(interface != NULL);
  // Ensure we have enough size of payload in our ethernet header
  if (len > max_payload_size || max_payload_size == 0) {
    if (eth_header != NULL)
      free(eth_header);
    eth_header       = ethernet_header_alloc(len);
    max_payload_size = len;
  }
  ethernet_header_set(eth_header, ETH_P_ARP, content, len, interface->mac,
                      dst_mac);
#if DEBUG
  printf("[DEBUG][SEND] Payload size=%u ", len);
  ethernet_fprint(eth_header, stdout);
  printf("\n");
#endif

  tun_write((char*)eth_header, len + ETHERNET_HEADER_SIZE);
}
//------------------------------------------------------------------------------
//                           ALLOCATION
//------------------------------------------------------------------------------
struct net_interface* net_interface_alloc(char* ip, char* mac) {
  struct net_interface* interface = calloc(1, sizeof(struct net_interface));
  memcpy(interface->mac, mac, 6);
  if (inet_pton(AF_INET, ip, &interface->ip) != 1) {
    ERROR("NET: Parsing inet address failed\n");
    exit(1);
  }
  return interface;
}
//------------------------------------------------------------------------------
//                           LIFECYCLE
//------------------------------------------------------------------------------
void net_interface_destroy() {
  if (eth_header != NULL)
    free(eth_header);
}

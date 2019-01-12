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

uint32_t net_interface_get_ip(struct net_interface* interface) {
  assert(interface != NULL);
  return interface->ip;
}

unsigned char* net_interface_get_mac(struct net_interface* interface) {
  assert(interface != NULL);
  return interface->mac;
}

static struct eth_header* header_buffer = NULL;

void net_interface_send(struct net_interface* interface, char* content,
                        uint32_t len, unsigned char* dst_mac) {
  assert(interface != NULL);
  assert(header_buffer != NULL);
  ethernet_header_set(header_buffer, ETH_P_ARP, content, len, interface->mac,
                      dst_mac);
  printf("[DEBUG][SEND] Payload size=%u ", len);
  ethernet_fprint(header_buffer, stdout);
  printf("\n");

  tun_write((char*)header_buffer, len + ETHERNET_HEADER_SIZE + 1);
}

struct net_interface* net_interface_alloc(char* ip, char* mac) {
  if (header_buffer == NULL) {
    header_buffer = ethernet_header_alloc();
  }

  struct net_interface* interface = calloc(1, sizeof(struct net_interface));
  memcpy(interface->mac, mac, 6);
  if (inet_pton(AF_INET, ip, &interface->ip) != 1) {
    ERROR("NET: Parsing inet address failed\n");
    exit(1);
  }
  return interface;
}

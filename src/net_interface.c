#include "net_interface.h"
#include <arpa/inet.h>
#include <assert.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define DEBUG 1

#include "./protocols/ethernet.h"
#include "./utils/protocols.h"
#include "tun_device.h"
#include "utils/log.h"

static uint32_t ip                            = 0;
static unsigned char mac[6]                   = {};
static struct eth_header* buffers[TUN_QUEUES] = {};
static uint32_t buffers_size[TUN_QUEUES]      = {};
static pthread_mutex_t mut_buffer             = PTHREAD_MUTEX_INITIALIZER;
static const unsigned char BROADCAST[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
//------------------------------------------------------------------------------
//                               GETTERS
//------------------------------------------------------------------------------
/**
  IN NETWORK BYTE ORDER
**/
uint32_t net_interface_get_ip() { return ip; }
unsigned char* net_interface_get_mac() { return mac; }

static void ensure_has_size(int fi, uint32_t size) {
  // Ensure we have enough size of payload in our ethernet header
  pthread_mutex_lock(&mut_buffer);
  if (size > buffers_size[fi] || buffers_size[fi] == 0) {
    if (buffers[fi] != NULL)
      buffers[fi] = realloc(buffers[fi], size);
    else
      buffers[fi] = calloc(1, size);
    buffers_size[fi] = size;
  }
  pthread_mutex_unlock(&mut_buffer);
}
//------------------------------------------------------------------------------
//                           SEND DATA
//------------------------------------------------------------------------------
void net_interface_send(const char* content, uint32_t len,
                        const unsigned char* dst_mac, uint16_t protocol) {
  assert(dst_mac != NULL);
  int fi              = available();
  uint32_t total_size = len + sizeof(struct eth_header);

  if (fi == -1) {
    ERROR("NET-IF: No tun device available.")
    abort();
  }

  ensure_has_size(fi, total_size);

  struct eth_header* eth_header = buffers[fi];

  // We copy the data
  eth_header->ether_type = protocol;
  memcpy(eth_header->dst_mac, dst_mac, 6);
  memcpy(eth_header->src_mac, mac, 6);
  memcpy(eth_header->payload, content, len);

#if DEBUG
  printf("[DEBUG] [NET-IF] {%d} Sent payload size=%u ", fi, len);
  ethernet_fprint(eth_header, stdout);
  printf("\n");
#endif

  eth_header_hton(eth_header);
  set_busy(fi, true);
  tun_write(fi, (char*)eth_header, total_size);
  set_busy(fi, false);
}
void net_interface_broadcast(const char* content, uint32_t len,
                             uint16_t protocol) {
  net_interface_send(content, len, BROADCAST, protocol);
}
//------------------------------------------------------------------------------
//                           RECEIVE DATA
//------------------------------------------------------------------------------
int net_interface_read(char* buffer, uint32_t len) {
  int fi  = available();
  int out = -1;
  if (fi == -1) {
    LOG_DEBUG("[NET-IF] No tun device available.");
    return out;
  }
  set_busy(fi, true);
  out = tun_read(fi, buffer, len);
  set_busy(fi, false);
  return out;
}
//------------------------------------------------------------------------------
//                           ALLOCATION
//------------------------------------------------------------------------------
void net_interface_init(const char* src_ip, const char* src_mac) {
  memcpy(mac, src_mac, 6);
  if (inet_pton(AF_INET, src_ip, &ip) != 1) {
    ERROR("NET-IF: Parsing inet address failed\n");
    exit(1);
  }
}
//------------------------------------------------------------------------------
//                           LIFECYCLE
//------------------------------------------------------------------------------
void net_interface_destroy() {
  for (int i = 0; i < TUN_QUEUES; i++) {
    struct eth_header* eth_header = buffers[i];
    if (eth_header != NULL)
      free(eth_header);
  }
}

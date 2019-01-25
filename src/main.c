#include <errno.h>
#include <linux/if_ether.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG 0

#include "./protocols/ethernet.h"
#include "net_interface.h"
#include "protocols/arp.h"
#include "protocols/ip.h"
#include "tun_device.h"
#include "utils/log.h"

#define BUFLEN 100

bool handle_frame(struct eth_header* header) {
  switch (header->ether_type) {
  case ETH_P_ARP:
#if DEBUG
    printf("[DEBUG] [ETH] Received (ARP) ");
    ethernet_fprint(header, stdout);
    printf("\n");
#endif
    arp_receive(header);
    return true;
  case ETH_P_IP:
#if DEBUG
    printf("[DEBUG] [ETH] Received (IP) ");
    ethernet_fprint(header, stdout);
    printf("\n");
#endif
    ip_receive(header);
    break;
  default:
    // ERROR("Unrecognized ethertype %u\n", ethernet_type(header));
    break;
  }
  return false;
}

void* process_io(void* buff) {
  char* buffer              = buff;
  struct eth_header* header = (struct eth_header*)(buffer);
  while (1) {
    if (net_interface_read(buffer, BUFLEN) < 0) {
      ERROR("Read from tun_fd: %s\n", strerror(errno));
    }
    header = (struct eth_header*)(buffer);
    eth_header_ntoh(header);
    handle_frame(header);
  }
  return NULL;
}

void destroy() {
  arp_destroy();
  net_interface_destroy();
}

#define THREADS 10
static pthread_t threads[THREADS] = {};
static char* buffers[THREADS]     = {};

int main(int argc, char** argv) {
  char dev[10] = {};
  atexit(&destroy);

  // Init everything
  tun_init(dev);
  INFO("Created TUN interface : %s\n", dev);
  net_interface_init("10.0.0.4", "00:0c:29:6d:50:25");
  arp_init();
  INFO("Init successful\n");

  for (int i = 0; i < THREADS; ++i) {
    buffers[i] = calloc(BUFLEN, sizeof(char));
    pthread_create(&(threads[i]), NULL, &process_io, buffers[i]);
  }

  while (1) {
    sleep(1);
  }

  destroy();
  return EXIT_SUCCESS;
}

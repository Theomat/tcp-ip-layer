#include <errno.h>
#include <linux/if_ether.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ethernet.h"
#include "net_interface.h"
#include "utils/log.h"

#include "arp.h"
#include "ip.h"
#include "tuntap_interface.h"

#define BUFLEN 100

bool handle_frame(struct net_interface* interface, struct eth_header* header) {

  switch (ethernet_get_type(header)) {
  case ETH_P_ARP:
#if DEBUG
    printf("[DEBUG][RECV] ");
    ethernet_fprint(header, stdout);
    printf("\n");
#endif
    arp_receive(interface, header);
    return true;
  case ETH_P_IP:
    LOG_DEBUG("Found IPv4\n");
    ip_receive(interface, header);
    break;
  default:
    // ERROR("Unrecognized ethertype %u\n", ethernet_type(header));
    break;
  }
  return false;
}

void destroy() {
  arp_destroy();
  net_interface_destroy();
}

int main(int argc, char** argv) {
  char buffer[BUFLEN] = {};
  char* dev           = calloc(10, 1);

  atexit(&destroy);

  // Init everything
  tun_init(dev);
  INFO("Created TUN interface : %s\n", dev);
  struct net_interface* interface =
      net_interface_alloc("10.0.0.4", "00:0c:29:6d:50:25");
  arp_init();
  INFO("Init successful\n");

  while (1) {
    if (tun_read(buffer, BUFLEN) < 0) {
      ERROR("Read from tun_fd: %s\n", strerror(errno));
    }

    struct eth_header* header = to_ethernet_header(buffer);
    handle_frame(interface, header);
  }
  // Destroy everything created
  arp_destroy();
  net_interface_destroy();
  return EXIT_SUCCESS;
}

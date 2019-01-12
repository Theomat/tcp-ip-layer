#include <errno.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "utils.h"

#include "tuntap_interface.h"

// Tun device file descriptor
static int tun_fd;

static int set_interface_route(char* dev, char* cidr) {
  return run_cmd("ip route add dev %s %s", dev, cidr);
}
static int set_interface_up(char* dev) {
  return run_cmd("ip link set dev %s up", dev);
}

/*
 * Taken from
 * https://github.com/torvalds/linux/blob/v4.4/Documentation/networking/tuntap.txt
 */
static int tun_alloc(char* dev) {
  struct ifreq ifr;
  int fd, err;

  if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
    ERROR("TUN: Cannot open TUN/TAP dev\n");
    exit(1);
  }

  PUT_ZEROES(ifr);

  /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
   *        IFF_TAP   - TAP device
   *
   *        IFF_NO_PI - Do not provide packet information
   */
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
  if (*dev) {
    // Return interface name in dev
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  if ((err = ioctl(fd, TUNSETIFF, (void*)&ifr)) < 0) {
    ERROR("TUN: Could not ioctl tun: %s\n", strerror(errno));
    close(fd);
    return err;
  }

  strcpy(dev, ifr.ifr_name);
  return fd;
}

int tun_read(char* buf, int len) { return read(tun_fd, buf, len); }

int tun_write(char* buf, int len) { return write(tun_fd, buf, len); }

void tun_init(char* dev) {
  tun_fd = tun_alloc(dev);

  if (set_interface_up(dev) != 0) {
    ERROR("TUN: Failed setting up interface\n");
  }

  if (set_interface_route(dev, "10.0.0.0/24") != 0) {
    ERROR("TUN: Failed setting route for interface\n");
  }
}

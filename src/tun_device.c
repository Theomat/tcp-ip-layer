#include "tun_device.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "utils/log.h"
#include "utils/runtime_utils.h"

// Tun device file descriptor
static int tun_fds[TUN_QUEUES]      = {};
static bool busy_status[TUN_QUEUES] = {};
static pthread_mutex_t mut_status   = PTHREAD_MUTEX_INITIALIZER;
//------------------------------------------------------------------------------
//                           SET UP INTERFACE
//------------------------------------------------------------------------------
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
static int tun_alloc(char* dev, int queues, int* fds) {
  struct ifreq ifr;
  int fd, err, i;

  if (dev == NULL) {
    return -1;
  }

  PUT_ZEROES(ifr);

  /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
   *        IFF_TAP   - TAP device
   *
   *        IFF_NO_PI - Do not provide packet information
   *        IFF_MULTI_QUEUE - Create a queue of multiqueue device
   */
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI | IFF_MULTI_QUEUE;

  for (i = 0; i < queues; i++) {
    if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
      goto err;
    err = ioctl(fd, TUNSETIFF, (void*)&ifr);
    if (err) {
      close(fd);
      goto err;
    }
    fds[i] = fd;
  }
  strcpy(dev, ifr.ifr_name);
  return 0;
err:
  ERROR("TUN: Could not ioctl tun: %s\n", strerror(errno));
  for (--i; i >= 0; i--)
    close(fds[i]);
  return err;
}
//------------------------------------------------------------------------------
//                               STATUS
//------------------------------------------------------------------------------
void set_busy(uint8_t fi, bool busy) {
  pthread_mutex_lock(&mut_status);
  busy_status[fi] = busy;
  pthread_mutex_unlock(&mut_status);
}
int8_t available() {
  pthread_mutex_lock(&mut_status);
  int8_t i = 0;
  while (busy_status[i])
    i++;
  pthread_mutex_unlock(&mut_status);
  if (i >= TUN_QUEUES)
    return -1;
  return i;
}
//------------------------------------------------------------------------------
//                               IO
//------------------------------------------------------------------------------
int tun_read(int8_t fi, char* buf, int len) {
  return read(tun_fds[fi], buf, len);
}
int tun_write(int8_t fi, char* buf, int len) {
  return write(tun_fds[fi], buf, len);
}
//------------------------------------------------------------------------------
//                           ALLOCATION
//------------------------------------------------------------------------------
void tun_init(char* dev) {
  if (tun_alloc(dev, TUN_QUEUES, tun_fds)) {
    ERROR("TUN: Failed allocation\n");
    abort();
  }

  if (set_interface_up(dev) != 0) {
    ERROR("TUN: Failed setting up interface: %s\n", dev);
    abort();
  }

  if (set_interface_route(dev, "10.0.0.0/24") != 0) {
    ERROR("TUN: Failed setting route for interface\n");
    abort();
  }
}

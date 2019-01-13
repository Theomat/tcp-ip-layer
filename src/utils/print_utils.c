#include <stdint.h>
#include <stdio.h>

#include "print_utils.h"

void mac_fprint(FILE* fd, unsigned char* mac) {
  fprintf(fd, "%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4],
          mac[5]);
}

void ipv4_fprint(FILE* fd, uint32_t ip) {
  fprintf(fd, "%u.%u.%u.%u", ((ip >> 24) & 255), ((ip >> 16) & 255),
          ((ip >> 8) & 255), ip & 255);
}

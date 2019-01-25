#include "ethernet.h"
#include <arpa/inet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/log.h"

//------------------------------------------------------------------------------
//                                PRINT
//------------------------------------------------------------------------------
void ethernet_fprint(struct eth_header* header, FILE* fd) {
  assert(header != NULL);
  fprintf(fd, "ETH header [dst=");
  mac_fprint(fd, header->dst_mac);
  fprintf(fd, ", src=");
  mac_fprint(fd, header->src_mac);
  fprintf(fd, ", type=%u]", header->ether_type);
}
//------------------------------------------------------------------------------
//                           CONVERTION
//------------------------------------------------------------------------------
void eth_header_hton(struct eth_header* header) {
  assert(header != NULL);
  header->ether_type = htons(header->ether_type);
}
void eth_header_ntoh(struct eth_header* header) {
  assert(header != NULL);
  header->ether_type = ntohs(header->ether_type);
}

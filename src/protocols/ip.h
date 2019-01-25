#include <stdint.h>
#include <stdio.h>

#include "ethernet.h"
#ifndef IP_H
#define IP_H

#define PROTOCOL_ICMP 1
#define PROTOCOL_TCP 6
#define PROTOCOL_UDP 16
//------------------------------------------------------------------------------
//                                  STRUCTURE
//------------------------------------------------------------------------------
struct ip_header {
  // Protocol version : 4 for IPv4 (1st four bytes)
  uint8_t version_and_header_len;
  // (2nd four bytes) Internet Header Length == number of 32-bit words in header
  // : 4 for IPv4

  // Type Of Service : (from 1st specification)
  uint8_t service_type;
  // Length of the whole IP datagram
  uint16_t len;
  // Used in fragmentation
  uint16_t index;
  // Used in fragmentation
  uint16_t flags_and_fragment_offset;
  // Position of a fragment in a IP datagram
  // Time to Live
  uint8_t ttl;
  // Protocol Type in payload: UDP 16, TCP 6, ICMP 1
  uint8_t protocol;
  // Checksum
  uint16_t checksum;
  uint32_t src_addr;
  uint32_t dst_addr;
  unsigned char payload[];
} __attribute__((packed));
//------------------------------------------------------------------------------
//                             GETTERS
//------------------------------------------------------------------------------
#define ip_header_get_version(header)                                          \
  ((header->version_and_header_len >> 4) & 0xf)

#define ip_header_get_header_length(header)                                    \
  (header->version_and_header_len & 0xf)

#define ip_header_get_payload_length(header)                                   \
  (header->len - sizeof(struct ip_header))

#define ip_header_payload(header)                                              \
  ((unsigned char*)header + sizeof(struct ip_header))
//------------------------------------------------------------------------------
//                             SETTERS
//------------------------------------------------------------------------------
#define ip_header_set_version(header, version)                                 \
  (header->version_and_header_len =                                            \
       (version << 4) + ip_header_get_header_length(header))

#define ip_header_set_header_length(header, ihl)                               \
  (header->version_and_header_len = (ip_header_get_version(header) << 4) + ihl)
//------------------------------------------------------------------------------
//                              PRINT
//------------------------------------------------------------------------------
void ip_header_fprint(FILE* fd, const struct ip_header* header);
//------------------------------------------------------------------------------
//                              RECEPTION
//------------------------------------------------------------------------------
void ip_receive(struct eth_header* eth_header);
//------------------------------------------------------------------------------
//                              SEND
//------------------------------------------------------------------------------
void ip_send_packet(struct ip_header* header);
void ip_send(uint32_t dst, char* payload, uint32_t len, uint32_t protocol);

#endif

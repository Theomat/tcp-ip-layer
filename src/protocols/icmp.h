#include <stdio.h>

#include "ip.h"

#ifndef ICMP_H
#define ICMP_H
//------------------------------------------------------------------------------
//                            STRUCTURES
//------------------------------------------------------------------------------
struct icmp_v4;
struct icmp_v4_echo;
struct icmp_v4_dst_unreachable;
void icmp_receive(struct ip_header* ip_header);

#endif

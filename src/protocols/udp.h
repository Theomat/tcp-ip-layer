#ifndef UDP_H
#define UDP_H
struct udp_header;
//------------------------------------------------------------------------------
//                              PRINT
//------------------------------------------------------------------------------
void udp_header_fprint(FILE* fd, struct udp_header* header);
//------------------------------------------------------------------------------
//                              RECEPTION
//------------------------------------------------------------------------------
void udp_receive(struct net_interface* interface, struct ip_header* ip_header);

#endif

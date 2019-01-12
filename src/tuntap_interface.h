#ifndef TUNTAP_INTERFACE_H
#define TUNTAP_INTERFACE_H
int tun_read(char* buf, int len);
int tun_write(char* buf, int len);
void tun_init(char* dev);

#endif

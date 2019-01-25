#include <stdint.h>
#include <stdio.h>
#ifndef LOG_H
#define LOG_H
void mac_fprint(FILE* fd, unsigned char* mac);
void ipv4_fprint(FILE* fd, uint32_t ip);

#define INFO(args...)                                                          \
  printf("[INFO] ");                                                           \
  printf(args);
#define ERROR(args...)                                                         \
  fprintf(stderr, "[ERROR] ");                                                 \
  fprintf(stderr, args);
#if DEBUG
#define LOG_DEBUG(args...)                                                     \
  printf("[DEBUG] ");                                                          \
  printf(args);
#else
#define LOG_DEBUG(args...)
#endif
#endif

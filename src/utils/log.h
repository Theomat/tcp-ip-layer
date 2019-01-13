#ifndef LOG_H
#define LOG_H

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

#ifndef UTILS_H
#define UTILS_H
int run_cmd(char* cmd, ...);

#define PUT_ZEROES(var) memset(&var, 0, sizeof(var))
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

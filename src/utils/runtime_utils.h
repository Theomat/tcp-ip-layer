#ifndef RUNTIME_UTILS_H
#define RUNTIME_UTILS_H
int run_cmd(char* cmd, ...);

#define PUT_ZEROES(var) memset(&var, 0, sizeof(var))
#endif

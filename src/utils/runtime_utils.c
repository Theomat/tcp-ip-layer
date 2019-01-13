#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "runtime_utils.h"

#define CMDBUFLEN 100

int run_cmd(char* cmd, ...) {
  va_list ap;
  char buf[CMDBUFLEN];
  va_start(ap, cmd);
  vsnprintf(buf, CMDBUFLEN, cmd, ap);

  va_end(ap);

  return system(buf);
}

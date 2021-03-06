#include "protocols.h"
/** Compute Internet sum for "count" bytes
 *         beginning at location "addr".
  Taken from https://tools.ietf.org/html/rfc1071
**/
uint16_t sum16(const void* addr, unsigned int count) {
  register uint32_t sum = 0;
  const uint16_t* ptr   = addr;

  while (count > 1) {
    /*  This is the inner loop */
    sum += *ptr++;
    count -= 2;
  }

  /*  Add left-over byte, if any */
  if (count > 0)
    sum += *(uint8_t*)ptr;

  /*  Fold 32-bit sum to 16 bits */
  while (sum >> 16)
    sum = (sum & 0xffff) + (sum >> 16);
  return sum;
}

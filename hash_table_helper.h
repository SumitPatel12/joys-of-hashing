#ifndef HASH_TABLE_HELPER
#define HASH_TABLE_HELPER

#include <stdlib.h>

// s can only be so big if we want to store it :shrug:
uint64_t
hash_bin_index(uint64_t x, uint8_t s) {
  uint64_t p = (uint64_t)(1 << s) - 1;
  uint64_t y = (x >> s) + (x & p);
  return (y > p) ? y - p : y;
}

#endif

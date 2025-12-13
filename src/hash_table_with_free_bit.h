#ifndef HASH_TABLE_WITH_FREE_BIT_H
#define HASH_TABLE_WITH_FREE_BIT_H

#include <stdint.h>
#include <stdlib.h>

struct bin {
  unsigned int is_free : 1;
  unsigned int key;
};

struct hash_table_with_free_bit {
  unsigned int size;
  uint8_t mersenne_prime_power;
  struct bin *bins;
};

struct hash_table_with_free_bit *
new_table_with_free_bit(uint8_t mersenne_prime_power, unsigned int size);

void
delete_table_with_free_bit(struct hash_table_with_free_bit *table);

struct bin *
hash_bin_with_free_bit(struct hash_table_with_free_bit *table, unsigned int key);

#endif

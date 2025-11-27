#ifndef HASH_TABLE_WITHOUT_FREE_BIT_H
#define HASH_TABLE_WITHOUT_FREE_BIT_H

#include <stdlib.h>

#include "hash_table_helper.h"

// So this is better, just assign a value as default which will signify that the
// bin is uninitialized.
#define DEFAULT_KEY (unsigned int)0

struct hash_table_without_free_bit {
  unsigned int size;
  uint8_t mersenne_prime_power;
  unsigned int *bins;
};

struct hash_table_without_free_bit *
new_table_without_free_bit(uint8_t mersenne_prime_power, unsigned int size) {
  struct hash_table_without_free_bit *table = (struct hash_table_without_free_bit *)malloc(sizeof *table);
  unsigned int *bins = (unsigned int *)malloc(size * sizeof *bins);

  // Sadly malloc can fail.
  if (!table || !bins) goto error;

  *table =
      (struct hash_table_without_free_bit){.size = size, .bins = bins, .mersenne_prime_power = mersenne_prime_power};

  unsigned int *beg = table->bins, *end = beg + size;
  for (unsigned int *bin = beg; bin != end; ++bin) {
    *bin = DEFAULT_KEY;
  }

  return table;

error:
  free(table);
  free(bins);
  return NULL;
}

void
delete_table_without_free_bit(struct hash_table_without_free_bit *table) {
  free(table->bins);
  free(table);
}

static inline unsigned int *
hash_bin(struct hash_table_without_free_bit *table, unsigned int key) {
  return (table->bins) + hash_bin_index(key, table->mersenne_prime_power);
}

#endif

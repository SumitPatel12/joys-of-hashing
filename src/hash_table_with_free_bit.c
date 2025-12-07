#include "hash_table_with_free_bit.h"

#include <stdlib.h>

struct hash_table_with_free_bit *
new_table_with_free_bit(uint8_t mersenne_prime_power, unsigned int size) {
  struct hash_table_with_free_bit *table = (struct hash_table_with_free_bit *)malloc(sizeof *table);
  struct bin *bins = (struct bin *)malloc(size * sizeof *bins);

  // Sadly malloc can fail.
  // TODO: Look into if goto is a good fit for error handling here.
  if (!table || !bins) goto error;

  *table = (struct hash_table_with_free_bit){.size = size, .bins = bins, .mersenne_prime_power = mersenne_prime_power};

  struct bin *beg = table->bins, *end = beg + size;
  for (struct bin *bin = beg; bin != end; ++bin) {
    bin->is_free = 1;
  }

  return table;

error:
  free(table);
  free(bins);
  return NULL;
}

void
delete_table_with_free_bit(struct hash_table_with_free_bit *table) {
  free(table->bins);
  free(table);
}

struct bin *
hash_bin_with_free_bit(struct hash_table_with_free_bit *table, unsigned int key) {
  return (table->bins) + hash_bin_index(key, table->mersenne_prime_power);
}

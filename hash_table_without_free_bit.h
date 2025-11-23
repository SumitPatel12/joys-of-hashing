#ifndef HASH_TABLE_WITHOUT_FREE_BIT_H
#define HASH_TABLE_WITHOUT_FREE_BIT_H

#include <stdlib.h>

// So this is better, just assign a value as default which will signify that the
// bin is uninitialized.
#define DEFAULT_KEY (unsigned int)0

struct hash_table_without_free_bit {
  unsigned int *bins;
  unsigned int size;
};

struct hash_table_without_free_bit *
new_table_without_free_bit(unsigned int size) {
  struct hash_table_without_free_bit *table =
      (struct hash_table_without_free_bit *)malloc(sizeof *table);
  unsigned int *bins = (unsigned int *)malloc(size * sizeof *bins);

  if (!table || !bins)
    goto error;

  *table = (struct hash_table_without_free_bit){.size = size, .bins = bins};

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

void delete_table_without_free_bit(struct hash_table_without_free_bit *table) {
  free(table->bins);
  free(table);
}

#endif

#ifndef HASH_TABLE_WITH_FREE_BIT_H
#define HASH_TABLE_WITH_FREE_BIT_H

#include <stdlib.h>

/*
 * While you could do something like this, it's space overhead is significant.
 * Reason being we can't just use one bit, so we go with an int, which depending
 * on the underlying hardware could be 4 bytes or 8 bytes. which is 31 or 63
 * wasted bits. Also alignment issues.
 */
struct bin {
  unsigned int is_free : 1;
  unsigned int key;
};

struct hash_table_with_free_bit {
  struct bin *bins;
  unsigned int size;
};

struct hash_table_with_free_bit *new_table_with_free_bit(unsigned int size) {
  struct hash_table_with_free_bit *table =
      (struct hash_table_with_free_bit *)malloc(sizeof *table);
  struct bin *bins = (struct bin *)malloc(size * sizeof *bins);

  if (!table || !bins)
    goto error;

  *table = (struct hash_table_with_free_bit){.size = size, .bins = bins};

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

void delete_table_with_free_bit(struct hash_table_with_free_bit *table) {
  free(table->bins);
  free(table);
}

#endif

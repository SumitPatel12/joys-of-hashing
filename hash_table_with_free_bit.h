#ifndef HASH_TABLE_WITH_FREE_BIT_H
#define HASH_TABLE_WITH_FREE_BIT_H

#include <stdint.h>
#include <stdlib.h>

#include "hash_table_helper.h"

/*
 * What should the API look like for a hash table?
 * 1. Constructor  - Creates and initializes the table.
 * 2. Destructor   - Of course deletes the table and frees any allocated space.
 * 3. Insert Key   - Inserts a key in the hash table.
 * 4. Delete Key   - Deletes a key from the hash table.
 * 5. Contains Key - Checks whether or not the given key is present in the hash table.
 * 6. Resize       - Resize the hash table.
 */

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
  unsigned int size;
  uint8_t mersenne_prime_power;
  struct bin *bins;
};

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

static inline struct bin *
hash_bin(struct hash_table_with_free_bit *table, unsigned int key) {
  return (table->bins) + hash_bin_index(key, table->mersenne_prime_power);
}

#endif

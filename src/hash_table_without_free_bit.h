#ifndef HASH_TABLE_WITHOUT_FREE_BIT_H
#define HASH_TABLE_WITHOUT_FREE_BIT_H

#include <assert.h>
#include <stdbool.h>
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

// So this is better, just assign a value as default which will signify that the
// bin is uninitialized.
// That's what I think of course, the free-bit might make sense for you. Moving forwards all implementations I've got
// will be on this one.
#define DEFAULT_KEY (unsigned int)0

struct hash_table {
  unsigned int size;
  uint8_t mersenne_prime_power;
  unsigned int *bins;
};

struct hash_table *
new_table_without_free_bit(uint8_t mersenne_prime_power, unsigned int size) {
  struct hash_table *table = (struct hash_table *)malloc(sizeof *table);
  unsigned int *bins = (unsigned int *)malloc(size * sizeof *bins);

  // Sadly malloc can fail.
  if (!table || !bins) goto error;

  *table = (struct hash_table){.size = size, .bins = bins, .mersenne_prime_power = mersenne_prime_power};

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
delete_table_without_free_bit(struct hash_table *table) {
  free(table->bins);
  free(table);
}

inline unsigned int *
hash_bin(struct hash_table *table, unsigned int key) {
  return (table->bins) + hash_bin_index(key, table->mersenne_prime_power);
}

int
insert_key(struct hash_table *table, unsigned int key) {
  if (key == DEFAULT_KEY) return -1;
  unsigned int *bin = hash_bin(table, key);

  if (*bin == DEFAULT_KEY) {
    *bin = key;
  } else {
    // TODO: Handle collissions.
  }

  return 0;
}

bool
contains_key(struct hash_table *table, unsigned int key) {
  // Well we don't need to check for default key cause we don't let people insert it.
  // And so it's guaranteed that the key will not match.
  unsigned int *bin = hash_bin(table, key);
  assert(*bin != DEFAULT_KEY);
  return *bin == key;
}

int
delete_key(struct hash_table *table, unsigned int key) {
  unsigned int *bin = hash_bin(table, key);
  assert(*bin != DEFAULT_KEY);
  if (*bin == key) {
    *bin = DEFAULT_KEY;
    return 0;
  } else {
    // TODO: Collission strikes back
  }
  return -1;
}

#endif

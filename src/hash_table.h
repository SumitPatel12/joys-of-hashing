#ifndef HASH_TABLE_H
#define HASH_TABLE_H

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

struct hash_table *new_table(uint8_t mersenne_prime_power, unsigned int size);

void delete_table(struct hash_table *table);

unsigned int *hash_bin(struct hash_table *table, unsigned int key);

int insert_key(struct hash_table *table, unsigned int key);

bool contains_key(struct hash_table *table, unsigned int key);

int delete_key(struct hash_table *table, unsigned int key);

#endif

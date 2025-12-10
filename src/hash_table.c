#include "hash_table.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "hash_table_helper.h"

struct hash_table *
new_table(uint8_t mersenne_prime_power, unsigned int size) {
  struct hash_table *table = (struct hash_table *)malloc(sizeof *table);
  if (!table) goto error;

  table->bins = malloc(size * sizeof *table->bins);
  table->size = size;

  // Sadly malloc can fail.
  if (!table->bins) goto error;

  for (LIST bin = table->bins; bin < table->bins + table->size; bin++) {
      *bin = NULL;
  }

  return table;

error:
for (LIST bin = table->bins; bin < table->bins + table->size; bin++) {
    free_list(bin);
}
  free(table);
  return NULL;
}

void
delete_table(struct hash_table *table) {
  free(table->bins);
  free(table);
}

LIST
get_bin_for_key(struct hash_table *table, unsigned int key) {
  return (table->bins) + hash_bin_index(key, table->mersenne_prime_power);
}

void
insert_key(struct hash_table *table, unsigned int key) {
    // TODO: Think of something better to do here.
    if (key == DEFAULT_KEY) return;

  LIST bin = get_bin_for_key(table, key);

  // No duplicates
  if (!contains_element(bin, key)) {
      add_element(bin, key);
  }
}

bool
contains_key(struct hash_table *table, unsigned int key) {
  // Well we don't need to check for default key cause we don't let people insert it.
  // And so it's guaranteed that the key will not match.
  return contains_element(get_bin_for_key(table, key), key);
}

void
delete_key(struct hash_table *table, unsigned int key) {
    LIST bin = get_bin_for_key(table, key);

    if (contains_element(bin, key)) {
        delete_element(bin, key);
    }
}

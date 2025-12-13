#ifndef OPEN_ADDRESSING_H
#define OPEN_ADDRESSING_H

#include <stdbool.h>
#include <stdlib.h>

struct bin {
  int is_free : 1;
  int is_deleted : 1;
  unsigned int key;
};

struct hash_table {
  struct bin *table;
  size_t size;
  uint8_t mersenne_prime_power;
#ifdef WITH_METRICS
  size_t collisions;
  size_t count;
#endif
};

struct hash_table *
empty_table(uint8_t mersenne_prime_power);
void
delete_table(struct hash_table *table);

void
insert_key(struct hash_table *table, unsigned int key);
bool
contains_key(struct hash_table *table, unsigned int key);
void
delete_key(struct hash_table *table, unsigned int key);

#ifdef WITH_METRICS
void
print_metrics(struct hash_table *table);
#endif

#endif

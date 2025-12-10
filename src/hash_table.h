#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

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

struct link {
  unsigned int key;
  struct link *next;
};

typedef struct link **LIST;

#define EMPTY_LIST &((struct link *){NULL})

struct hash_table {
  unsigned int size;
  uint8_t mersenne_prime_power;
  LIST bins;
};

static inline void
free_head(LIST list) {
  struct link *next = (*list)->next;
  free(*list);
  *list = next;
}

void
free_list(LIST list) {
  while (*list) {
    free_head(list);
  }
}

static inline LIST
create_owned_list() {
  struct link **ptr = malloc(sizeof *ptr);
  *ptr = NULL;
  return ptr;
};

static inline void
free_owned_list(LIST list) {
  free_list(list);
  free(list);
}

static inline struct link *
new_link(unsigned int key, struct link *next) {
  struct link *link = malloc(sizeof *link);
  *link = (struct link){.key = key, .next = next};
  return link;
}

static inline void
add_element(LIST list, unsigned int key) {
  *list = new_link(key, *list);
}

static inline LIST
find_key(LIST list, unsigned int key) {
  for (; *list; list = &(*list)->next) {
    if ((*list)->key == key) {
      return list;
    }
  }
  return NULL;
}

static inline bool
contains_element(LIST list, unsigned int key) {
  return find_key(list, key) != NULL;
}

static inline void
delete_element(LIST list, unsigned int key) {
  if ((list = find_key(list, key))) {
    free_head(list);
  }
}

struct hash_table *
new_table(uint8_t mersenne_prime_power, unsigned int size);

void
delete_table(struct hash_table *table);

LIST
get_bin_for_key(struct hash_table *table, unsigned int key);

void
insert_key(struct hash_table *table, unsigned int key);

bool
contains_key(struct hash_table *table, unsigned int key);

void
delete_key(struct hash_table *table, unsigned int key);

#endif

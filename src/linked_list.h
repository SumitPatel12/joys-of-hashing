#ifndef LINKED_LIST_FOR_HASH_TABLE_H
#define LINKED_LIST_FOR_HASH_TABLE_H

struct linked_list {
  unsigned int key;
  struct linked_list* next;
};

typedef struct linked_list** LIST;

#endif

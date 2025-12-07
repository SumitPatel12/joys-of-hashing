#ifndef LINKED_LIST_FOR_HASH_TABLE_H
#define LINKED_LIST_FOR_HASH_TABLE_H

#include <stddef.h>

/*
 The author suggested something like this which would work well no doubt.

 struct linked_list {
   unsigned int key;
   struct linked_list* next;
 };

 typedef struct linked_list** LIST;
 But I checkted the implementation of linux and BSD and they use something similar but with a special head node to save
 one pointers worth of space. Since I now know that's the better approach I'll go with that.

 Linux Impl Link: https://lxr.linux.no/linux+v2.6.34/include/linux/list.h#L554
 BSD Impl Link  : https://web.mit.edu/freebsd/head/sys/sys/queue.h (For this one search LIST_HEAD)

 I'll be going the Linux route for this one.
 Let me tell you, it really does seem easy when you read from the book, actual implementation on the other hand
 :woozy_face:
 */

// It's not a doubly linked list per say. The node does have a **prev but not the normal back traversal. Since we're
// using this for a hash map bin, where ordering won't matter, back traversal is very unlikely. The other thing is when
// you have a very large number of bins, saving the memory footprint of even one pointer will make a difference. You
// can't access the tail in O(1) though.
//
// This is an *intrusive* linked list, i.e. it is embedded into the structure. You can observe that the head and node
// don't store the key or the value, that is going to be in the struct we use. The struct will have the hlist_node
// embedded into it.

// Points to the first node in the list.
struct hlist_head {
  struct hlist_node* first;
};

struct hlist_node {
  // The next node in the list
  struct hlist_node* next;

  // It is a pointer to the pointer that points to this node. Not to be confused with a pointer to it's predecessor.
  // **prev does not store &&previous_node, it stores &(previous_node->next)
  //
  // If it's the first node, pprev would point to the head->first, i.e. pprev = &(head_node->first),
  // If it's one of the other nodes then it points to the previous_node->next, i.e. pprev = &(previous_node->next).
  // (I know, :woozy_face:x2)
  struct hlist_node** pprev;
};

// Initialize an empty hlist_head.
#define HLIST_HEAD_INIT {.first = NULL}
// Declare and initialize an empty hlist_head.
#define HLIST_HEAD(head_name) struct hlist_head head_name = {.first = NULL}
// Given a pointer to the hlist_head set's the hlist_head->first to NULL.
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

// Initialize an empty hlist_node.
static inline void
INIT_HLIST_NODE(struct hlist_node* n) {
  n->next = NULL;
  n->pprev = NULL;
};

// Returns whether the list is empty or not.
static inline int
hlist_empty(const struct hlist_head* h) {
  return !h->first;
};

// Deletes the given node from the list in O(1)
static inline void
hlist_delete(struct hlist_node* n) {
  struct hlist_node* next = n->next;
  // This refers to &(previous->next)
  struct hlist_node** pprev = n->pprev;

  // This is equivalent to previous->next = next;
  *pprev = next;
  if (next) {
    // And now this is equivalent to next->pprev = &(previous->next)
    // You see it points to the pointer pointing to itself.
    next->pprev = pprev;
  }
};

// Add head_node to the hlist
static inline void
hlist_add_head(struct hlist_node* n, struct hlist_head* h) {
  struct hlist_node* first = h->first;
  n->next = first;
  if (first) {
    first->pprev = &n->next;
  }

  h->first = n;
  n->pprev = &h->first;
};

#endif

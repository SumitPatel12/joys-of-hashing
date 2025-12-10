#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> // Required for offsetof

/* =========================================================
 * PART 1: The HList Implementation
 * ========================================================= */

struct hlist_head {
  struct hlist_node* first;
};

struct hlist_node {
  struct hlist_node* next;
  struct hlist_node** pprev;
};

// Initialize an empty hlist_head.
#define HLIST_HEAD_INIT {.first = NULL}
#define HLIST_HEAD(head_name) struct hlist_head head_name = {.first = NULL}
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

static inline void INIT_HLIST_NODE(struct hlist_node* n) {
  n->next = NULL;
  n->pprev = NULL;
};

static inline int hlist_empty(const struct hlist_head* h) {
  return !h->first;
};

static inline void hlist_delete_node(struct hlist_node* n) {
  struct hlist_node* next = n->next;
  struct hlist_node** pprev = n->pprev;

  *pprev = next;
  if (next) {
    next->pprev = pprev;
  }
};

static inline void hlist_add_head(struct hlist_node* n, struct hlist_head* h) {
  struct hlist_node* first = h->first;
  n->next = first;
  if (first) {
    first->pprev = &n->next;
  }
  h->first = n;
  n->pprev = &h->first;
};

/* =========================================================
 * PART 2: Helper Macros
 * ========================================================= */

#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

/* =========================================================
 * PART 3: Test Implementation
 * ========================================================= */

struct entry {
    int id;
    struct hlist_node node;
};

// Helper function to print the list state to avoid code duplication
void print_list_state(struct hlist_head *head, const char *label) {
    printf("\n=== POINTER OBSERVATION (%s) ===\n", label);
    printf("HEAD Struct Address:      %p\n", (void*)head);
    printf("HEAD.first points to:     %p\n", (void*)head->first);
    printf("HEAD.first lives at:      %p  <-- (A)\n", (void*)&head->first);
    printf("--------------------------------------------------\n");

    struct hlist_node *pos = head->first;

    while (pos) {
        struct entry *parent = container_of(pos, struct entry, node);

        printf("[Node ID: %d] Address:    %p\n", parent->id, (void*)pos);

        // 1. Where does pprev point?
        printf("  1. pos->pprev:          %p", (void*)pos->pprev);

        if (pos->pprev == &head->first) {
            printf("  (Matches A: Points to Head.first)\n");
        } else {
            printf("\n");
        }

        // 2. Dereference pprev
        printf("  2. *pos->pprev:         %p (Should match THIS Node Address)\n", (void*)*(pos->pprev));

        // 3. Next pointer address
        printf("  3. Address of pos->next:%p  <-- (B) (Next node's pprev will point here)\n", (void*)&pos->next);

        printf("\n");
        pos = pos->next;
    }
}

int main() {
    struct hlist_head head;
    INIT_HLIST_HEAD(&head);

    struct entry e1, e2, e3;
    e1.id = 1; INIT_HLIST_NODE(&e1.node);
    e2.id = 2; INIT_HLIST_NODE(&e2.node);
    e3.id = 3; INIT_HLIST_NODE(&e3.node);

    printf("--- Building List (Head -> e3 -> e2 -> e1) ---\n");
    hlist_add_head(&e1.node, &head);
    hlist_add_head(&e2.node, &head);
    hlist_add_head(&e3.node, &head);

    // Print State BEFORE Deletion
    print_list_state(&head, "BEFORE DELETE");

    // Test Deletion (Delete middle node e2)
    printf("--- Deleting Node ID 2 (The Middle Node) ---\n");
    hlist_delete_node(&e2.node);

    // Print State AFTER Deletion
    print_list_state(&head, "AFTER DELETE");

    return 0;
}

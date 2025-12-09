# `hlist` (Hash List) in the Linux Kernel

The `hlist` (hash list) is a fundamental data structure in the Linux kernel, specifically designed for efficient collision resolution in hash tables. Unlike traditional doubly-linked lists that require two pointers per node, `hlist` uses an optimized single-pointer head structure to reduce memory overhead in hash table implementations.

## Core Data Structures

The `hlist` implementation consists of two key structures defined in [`include/linux/types.h`](https://github.com/torvalds/linux/blob/master/include/linux/types.h):

```c
struct hlist_head {
    struct hlist_node *first;
};

struct hlist_node {
    struct hlist_node *next, **pprev;
};
```

**Key characteristics:**

- **`hlist_head`**: Contains only a single pointer to the first node, requiring just 8 bytes on 64-bit systems (compared to 16 bytes for a traditional `list_head` with both `next` and `prev` pointers).
- **`hlist_node`**: Embeds into data structures using the intrusive pattern—no separate allocation needed.
- **`pprev` pointer**: A pointer-to-pointer that points to the `next` field of the previous node (or the `first` field of the head). This enables O(1) deletion without traversing to find the predecessor.

This design is particularly memory-efficient for hash tables where many buckets may be empty or sparsely populated.

## Intrusive Nature

`hlist` is an **intrusive list structure**, meaning the list node is embedded directly within the data structure rather than allocating separate node objects.

**Usage pattern:**

```c
struct my_data {
    int key;
    char value[64];
    struct hlist_node hash_node;  // Embedded node
};
```

Access container structures using the `hlist_entry()` macro (implemented via `container_of()`):

```c
#define hlist_entry(ptr, type, member) \
    container_of(ptr, type, member)

struct my_data *data = hlist_entry(node, struct my_data, hash_node);
```

## Core Methods

### Initialization

[Source: `include/linux/list.h`](https://github.com/torvalds/linux/blob/master/include/linux/list.h)

```c
#define HLIST_HEAD_INIT { .first = NULL }
#define HLIST_HEAD(name) struct hlist_head name = { .first = NULL }
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

static inline void INIT_HLIST_NODE(struct hlist_node *h)
{
    h->next = NULL;
    h->pprev = NULL;
}
```

### Addition Operations

- **`hlist_add_head(n, h)`**: Add a new entry `n` at the beginning of the list `h` (O(1) operation).
- **`hlist_add_before(n, next)`**: Add `n` before the specified node `next`.
- **`hlist_add_behind(n, prev)`**: Add `n` after the specified node `prev`.
- **`hlist_add_fake(n)`**: Create a fake single-node hlist (for special cases).

### Deletion Operations

The `pprev` pointer design enables deletion without traversal—both previous and next are directly accessible via one pointer dereference.

```c
static inline void hlist_del(struct hlist_node *n)
{
    __hlist_del(n);
    n->next = LIST_POISON1;      // Poison pointers for debugging
    n->pprev = LIST_POISON2;
}
```

### Iteration Macros

- **`hlist_for_each(pos, head)`**: Iterate over list nodes.
- **`hlist_for_each_safe(pos, n, head)`**: Iterate safely against removal.
- **`hlist_for_each_entry(pos, head, member)`**: Iterate over container structures.
- **`hlist_for_each_entry_safe(pos, n, head, member)`**: Iterate over container structures safely against removal.

## Hash Table Usage

The primary use case for `hlist` is resolving hash table collisions through chaining. The kernel provides [`include/linux/hashtable.h`](https://github.com/torvalds/linux/blob/master/include/linux/hashtable.h) with convenient macros:

```c
#define DEFINE_HASHTABLE(name, bits) \
    struct hlist_head name[1 << (bits)] = \
        { [0 ... ((1 << (bits)) - 1)] = HLIST_HEAD_INIT }

#define hash_init(hashtable) \
    __hash_init(hashtable, HASH_SIZE(hashtable))

#define hash_add(hashtable, node, key) \
    hlist_add_head(node, &hashtable[hash_min(key, HASH_BITS(hashtable))])
```

**Workflow:**
1.  **Hash Function**: Maps key to a bucket index.
2.  **Collision Chain**: Uses `hlist` to resolve multiple entries in the same bucket.
3.  **Concurrency**: Often protected by per-bucket spinlocks or RCU.

## Lifecycle and Destruction

Destroying `hlist` structures depends on the synchronization mechanism used.

### 1. Non-RCU Destruction (Simple Case)

When elements are removed individually (e.g., protected by locks):

```c
hlist_del_init(&node->hash_node);
kfree(node);  // Safe to free immediately
```

To destroy an entire table, one typically iterates over all buckets and frees elements.

### 2. RCU-Protected Destruction

For structures using RCU (Read-Copy-Update), destruction requires a grace period to ensure all readers are done.

```c
hlist_del_rcu(&node->hash_node);
call_rcu(&node->rcu_head, destroy_callback);
```

The `call_rcu()` function defers the actual memory freeing until the RCU grace period has passed, preventing use-after-free bugs for concurrent readers.

## Usage in Other Projects

While the `hlist` design (single pointer head, pprev in node) is highly optimized for the Linux kernel's specific needs, other high-performance projects use different trade-offs:

-   **Redis**: Uses a standard doubly-linked list (`adlist.h`) with both `prev` and `next` pointers. It is non-intrusive (stores value pointer). This trades memory efficiency for simplicity and API friendliness.
-   **Nginx**: Uses a linked array of element pools (`ngx_list_part_t`). This prioritizes batch allocation and cache locality for sequential access over individual node manipulation.

The Linux `hlist` remains unique in its specific optimization for minimizing hash table bucket size (8 bytes vs 16 bytes) while maintaining O(1) removal capability.

## Deep Dive: `container_of`, `hlist_entry`, and Hash Table Internals

To fully understand how `hlist` works in practice, we must look at the macro magic that powers it (`container_of`) and the specific implementation details of the kernel's hash table.

### 1. The `container_of` Macro

The `container_of` macro is the mechanism that allows "intrusive" data structures to work. It recovers the pointer to the parent structure given a pointer to one of its members.

**Definition (`include/linux/container_of.h`):**

```c
#define container_of(ptr, type, member) ({				\
	void *__mptr = (void *)(ptr);					\
	static_assert(__same_type(*(ptr), ((type *)0)->member) ||	\
		      __same_type(*(ptr), void),			\
		      "pointer type mismatch in container_of()");	\
	((type *)(__mptr - offsetof(type, member))); })
```

**How it works:**
1.  **Type Checking**: The `static_assert` ensures at compile-time that the pointer you passed (`ptr`) matches the type of the member you specified. This prevents dangerous mismatches.
2.  **Pointer Arithmetic**: It casts the member pointer to `void *` to allow byte-level arithmetic.
3.  **Offset Calculation**: It subtracts the `offsetof(type, member)` from the pointer address. `offsetof` is a compiler built-in that returns the byte offset of a member from the start of the struct.
4.  **Result**: The result is the memory address of the beginning of the parent structure.

**Visualizing the Math:**

```
struct my_data {
    int id;                // offset 0
    struct hlist_node node;// offset 4 (assuming 4-byte int)
};

If we have a pointer 'ptr' to 'node':
Base Address = ptr - 4
```

### 2. `hlist_entry`

`hlist_entry` is simply a wrapper around `container_of` specifically for `hlist_node` pointers.

```c
#define hlist_entry(ptr, type, member) container_of(ptr, type, member)
```

It is used primarily during iteration to convert the `struct hlist_node *` back into your custom data structure pointer.

### 3. Hash Table Implementation (`include/linux/hashtable.h`)

The kernel's hash table implementation is built directly on top of `hlist`. It is a **fixed-size**, **array-based** hash table that uses chaining for collision resolution.

#### Defining a Hash Table

The table is defined using `DEFINE_HASHTABLE(name, bits)`.

```c
#define DEFINE_HASHTABLE(name, bits) \
    struct hlist_head name[1 << (bits)] = \
        { [0 ... ((1 << (bits)) - 1)] = HLIST_HEAD_INIT }
```

*   **Size**: The size is always a power of 2 (`1 << bits`). For example, `bits=10` creates an array of 1024 `hlist_head` entries.
*   **Memory**: Because `hlist_head` is just a single pointer, a 1024-entry table takes only 8KB (on 64-bit).
*   **Fixed Size**: The kernel hash table **does not resize** automatically. The size must be chosen at compile time based on expected load.

#### Hashing Functions (`hash_32` / `hash_64`)

The kernel uses multiplicative hashing based on the **Golden Ratio** to ensure good distribution of bits.

*   **Constants**: `0x61C88647` (32-bit) and `0x61C8864680B583EB` (64-bit). These are derived from `(phi^-1) * 2^w`.
*   **Mechanism**:
    1.  Multiply the key by the golden ratio constant.
    2.  Shift the result right to keep only the highest `bits` (the most random part of the product).

```c
// simplified view of hash_32
u32 hash_32(u32 val, unsigned int bits) {
    return (val * 0x61C88647) >> (32 - bits);
}
```

#### Bucket Selection and Addition

When you call `hash_add(hashtable, node, key)`:

1.  **Hash**: The key is hashed using `hash_32` or `hash_64` (selected via `hash_min` macro).
2.  **Index**: The result is used as the index into the `hlist_head` array.
3.  **Insert**: The node is added to the **head** of that bucket's list (`hlist_add_head`).

#### Iteration Logic

Iteration happens via `hash_for_each` or `hash_for_each_possible`.

*   **`hash_for_each(name, bkt, obj, member)`**: Iterates over **every bucket** (0 to size-1) and every node in each bucket. Used for cleanup or full traversals.
*   **`hash_for_each_possible(name, obj, member, key)`**:
    1.  Hashes the `key` to find the specific bucket.
    2.  Iterates **only** through that single chain.
    3.  You must manually check inside the loop if `obj->key == key` (since collisions are possible).

```c
hash_for_each_possible(my_table, obj, node, key) {
    if (obj->key == key)
        return obj;
}
```

## Dynamic Resizing: `rhashtable` vs. Manual Implementation

The standard `DEFINE_HASHTABLE` macro creates a **fixed-size** table. For dynamic workloads, the Linux kernel provides a dedicated solution: `rhashtable`.

### 1. The Kernel Solution: `rhashtable`

`rhashtable` (Resizable Hash Table) in `include/linux/rhashtable.h` is a production-grade implementation designed for high-performance concurrent scenarios.

**Key Architectural Differences:**

| Feature | `DEFINE_HASHTABLE` | `rhashtable` |
|:---|:---|:---|
| **Structure** | `struct hlist_head[1 << bits]` | Dynamic bucket tables with pointer chaining |
| **Node Type** | `hlist_node` (doubly-linked) | `rhash_head` (single-linked) |
| **Sizing** | Fixed at compile time | Dynamic (grows >75%, shrinks <30%) |
| **Locking** | Manual (usually per-bucket spinlock) | Internal per-bucket bit spinlocks + RCU |
| **Resizing** | N/A | Asynchronous & Non-blocking |

#### How `rhashtable` Works

It does **not** use `hlist`. Instead, it uses a custom single-linked list (`struct rhash_head`) to save memory and simplify the resizing logic.

**Resizing Mechanism:**
1.  **Trigger**: When load exceeds 75%, a resize is scheduled to a workqueue (asynchronous).
2.  **Allocation**: A new bucket table (double size) is allocated.
3.  **Future Table**: The old table points to the new one via a `future_tbl` pointer, allowing the two to coexist.
4.  **Incremental Rehash**: A worker thread incrementally moves entries from old buckets to new buckets.
5.  **Readers**: Concurrent readers can safely traverse both tables using RCU protection. If an entry moves while they are searching, they can follow the `future_tbl` pointer.

This complex architecture allows `rhashtable` to resize without ever blocking readers or stopping the world.

### 2. Manual Resizing (The "Stop-the-World" Approach)

If you strictly need to use `hlist` and `DEFINE_HASHTABLE` but want resizing, you must implement it manually. The standard approach is costly:

**Algorithm:**
1.  **Allocate**: Create a new table with double the size.
2.  **Lock Global**: Acquire a coarse-grained lock (blocking ALL reads/writes).
3.  **Rehash Loop**:
    ```c
    // Pseudo-code for manual resizing
    for (i = 0; i < old_size; i++) {
        hlist_for_each_safe(node, tmp, &old_table[i]) {
            hlist_del(node);
            new_idx = hash_min(key, new_bits);
            hlist_add_head(node, &new_table[new_idx]);
        }
    }
    ```
4.  **Swap & Free**: Update the global pointer and free the old table.
5.  **Unlock**: Resume operations.

**Why this is discouraged:**
*   It introduces a massive latency spike (O(N) operation under lock).
*   It destroys concurrency.
*   It is prone to allocation failures during the critical section.

**Summary**: Use `DEFINE_HASHTABLE` for static, predictable workloads. Use `rhashtable` for everything else.


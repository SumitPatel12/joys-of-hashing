#include "open_addressing.h"
#include <stdlib.h>
#include <string.h>
#include "hash_table_helper.h"

// Define this if you want to use LINEAR_PROBING, otherwise DOUBLE_HASHING
#define LINEAR_PROBING


#ifdef LINEAR_PROBING
// Linear probing with Mersenne Prime Modulo
// Index = (k + i) % M
static unsigned int
p(unsigned int k, unsigned int i, uint8_t s)
{
    return (unsigned int)hash_bin_index(k + i, s);
}
#else
// Double hashing with Mersenne Prime Modulo
// Index = (h1(k) + i * h2(k)) % M
static unsigned int
p(unsigned int k, unsigned int i, uint8_t s)
{
    unsigned int h1 = k;
    unsigned int h2 = (k << 1) | 1;
    // We can use the property that (a + b) % M = ((a % M) + (b % M)) % M
    // But hash_bin_index takes any uint64_t and mods it by (2^s - 1).
    // So we can just pass the sum/product directly if it fits in 64 bits.
    // However, h1 + i*h2 can overflow 32-bit int, but fits in 64-bit easily.
    return (unsigned int)hash_bin_index((uint64_t)h1 + (uint64_t)i * h2, s);
}
#endif

struct hash_table *
empty_table(uint8_t mersenne_prime_power)
{
    struct hash_table *table =
        (struct hash_table*)malloc(sizeof(struct hash_table));

    // Size M = 2^s - 1
    // We use 1ULL to ensure 64-bit shift, though size_t might be 32-bit (unlikely on modern systems but safe)
    size_t size = (1ULL << mersenne_prime_power) - 1;

    table->table = (struct bin *)malloc(size * sizeof(struct bin));
    for (size_t i = 0; i < size; ++i) {
        struct bin *bin = & table->table[i];
        bin->is_free = true;
        bin->is_deleted = false;
    }
    table->size = size;
    table->mersenne_prime_power = mersenne_prime_power;

#ifdef WITH_METRICS
    table->collisions = 0;
    table->count = 0;
#endif

    return table;
}

void
delete_table(struct hash_table *table)
{
    free(table->table);
    free(table);
}

void
insert_key(struct hash_table *table, unsigned int key)
{
    unsigned int index;
    struct bin *bin;
    for (size_t i = 0; i < table->size; ++i) {
        index = p(key, i, table->mersenne_prime_power);
        bin = & table->table[index];

#ifdef WITH_METRICS
        if (!bin->is_free && !bin->is_deleted && bin->key != key) {
            // It's occupied by someone else -> Collision
            table->collisions++;
        }
#endif

        if (bin->is_free || bin->is_deleted || bin->key == key)
            break;
    }
    
    // Check if we found a spot or if it's already there
    // If it was deleted, we are reusing it.
    // If it was free, we are using it.
    // If it was same key, we update (no-op here).
    
#ifdef WITH_METRICS
    if (bin->is_free || bin->is_deleted) {
        // Only increment count if we are actually taking a new slot
        // If it was already there (bin->key == key), don't increment.
        // BUT wait, we broke loop.
        // If bin->key == key and !is_deleted, it's a duplicate.
        // If bin->is_deleted, we overwrite.
        // If bin->is_free, we write.
        table->count++;
    } else if (bin->key == key) {
        // Duplicate, do nothing to count
    }
#endif

    bin->is_free = bin->is_deleted = false;
    bin->key = key;
}

bool
contains_key(struct hash_table *table, unsigned int key)
{
    for (size_t i = 0; i < table->size; ++i) {
        unsigned int index = p(key, i, table->mersenne_prime_power);
        struct bin *bin = & table->table[index];
        if (bin->is_free)
            return false;
        if (!bin->is_deleted && bin->key == key)
            return true;
    }
    return false;
}

void
delete_key(struct hash_table *table, unsigned int key)
{
    for (size_t i = 0; i < table->size; ++i) {
        unsigned int index = p(key, i, table->mersenne_prime_power);
        struct bin *bin = & table->table[index];
        if (bin->is_free) return;
        if (bin->key == key) {
            bin->is_deleted = true;
#ifdef WITH_METRICS
            table->count--; // Technically we should decrease count? 
                            // User asked to track load factor, so yes.
            // But main.c only does insertions, so delete might not be tested. 
            // Keeping it consistent anyway.
#endif
            return;
        }
    }
}

#ifdef WITH_METRICS
#include <stdio.h>
void
print_metrics(struct hash_table *table) {
    printf("Total stats:\n");
    printf("     Count: %zu\n", table->count);
    printf("Collisions: %zu\n", table->collisions);
}
#endif

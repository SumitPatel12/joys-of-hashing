#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

#ifdef USE_CHAINING
#include "../src/hash_table.h"
#define TABLE_NAME "Chaining"
#else
#include "../src/open_addressing.h"
#define TABLE_NAME "Open Addressing"
#endif

// PRNG for consistent benchmarks across runs with same seed
static uint64_t xorshift64(uint64_t *state) {
    uint64_t x = *state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return *state = x;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <seed> <num_items> <mersenne_power>\n", argv[0]);
        return 1;
    }

    uint64_t seed = strtoull(argv[1], NULL, 10);
    size_t num_items = strtoull(argv[2], NULL, 10);
    uint8_t mersenne_power = (uint8_t)strtoull(argv[3], NULL, 10);
    
    // Initialize table
#ifdef USE_CHAINING
    struct hash_table *table = new_table(mersenne_power, (1ULL<<mersenne_power)-1);
#else
    struct hash_table *table = empty_table(mersenne_power);
#endif

    if (!table) {
        fprintf(stderr, "Failed to allocate table\n");
        return 1;
    }

    // Generate Data
    // We generate data FIRST so generation time isn't included in insert time.
    unsigned int *keys = malloc(num_items * sizeof(unsigned int));
    if (!keys) {
        fprintf(stderr, "Failed to allocate keys\n");
        return 1;
    }

    uint64_t rng_state = seed;
    for (size_t i = 0; i < num_items; ++i) {
        keys[i] = (unsigned int)xorshift64(&rng_state);
        // Ensure non-zero keys if 0 is reserved (hash_table.c doesn't allow 0)
        if (keys[i] == 0) keys[i] = 1; 
    }

    // Benchmark Insertion
    clock_t start = clock();
    for (size_t i = 0; i < num_items; ++i) {
        insert_key(table, keys[i]);
    }
    clock_t end = clock();
    
    double insert_time = (double)(end - start) / CLOCKS_PER_SEC;

    // Benchmark Lookup
    // We look up every key we inserted (Hit case)
    clock_t val_start = clock();
    volatile int hits = 0; // Prevent optimization
    for (size_t i = 0; i < num_items; ++i) {
        if (contains_key(table, keys[i])) {
            hits++;
        }
    }
    clock_t val_end = clock();
    double lookup_time = (double)(val_end - val_start) / CLOCKS_PER_SEC;

    // Output Metrics
    // Format: Name, InsertTime, LookupTime, Count, Collisions
#ifdef WITH_METRICS
    printf("BENCH,%s,%f,%f,%zu,%zu\n", TABLE_NAME, insert_time, lookup_time, table->count, table->collisions);
#else
    printf("BENCH,%s,%f,%f,N/A,N/A\n", TABLE_NAME, insert_time, lookup_time);
#endif

    // Clean up
    free(keys);
#ifdef USE_CHAINING
    delete_table(table);
#else
    delete_table(table);
#endif

    return 0;
}

#include <stdio.h>
#include "hash_table.h"
#include "../benchmarks/modulo_vs_bitshift_benchmark.h"

int main() {
    // Intersting to note that without the \n at the end, I got an output of: "Maybe Hash Table!!%".
    // The trailing % at the end is a  zsh thing, it indicates that we didn't end in a new line.
    // :woozy_face:
    printf("Maybe Hash Table!!\n");
    printf("DEAFULT_KEY: %u\n", DEFAULT_KEY);

    // Run the modulo vs bit-shift benchmark
    run_modulo_vs_bitshift_benchmark(DEFAULT_ITERATIONS);

    return 0;
}

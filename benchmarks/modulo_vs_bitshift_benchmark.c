#include "modulo_vs_bitshift_benchmark.h"

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "../src/hash_table_helper.h"

double
get_time_ms(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

benchmark_run_t
benchmark_non_prime_modulo(uint64_t iterations) {
  printf("Benchmarking non-prime modulo...\n");
  double start = get_time_ms();
  uint64_t result = 0;

  for (uint64_t i = 0; i < iterations; i++) {
    result += i % NON_PRIME_SIZE;
  }

  benchmark_run_t run = {get_time_ms() - start, result};
  return run;
}

benchmark_run_t
benchmark_power_of_two_bitmask(uint64_t iterations) {
  printf("Benchmarking power of two bitmask...\n");
  double start = get_time_ms();
  uint64_t result = 0;
  uint64_t mask = POWER_OF_TWO_SIZE - 1;

  for (uint64_t i = 0; i < iterations; i++) {
    result += i & mask;
  }

  benchmark_run_t run = {get_time_ms() - start, result};
  return run;
}

benchmark_run_t
benchmark_mersenne_prime_bitshift(uint64_t iterations) {
  printf("Benchmarking Mersenne prime bit-shift (hash_bin_index)...\n");
  double start = get_time_ms();
  uint64_t result = 0;

  for (uint64_t i = 0; i < iterations; i++) {
    result += hash_bin_index(i, POWER_OF_TWO_SHIFT);
  }

  benchmark_run_t run = {get_time_ms() - start, result};
  return run;
}

benchmark_run_t
benchmark_mersenne_modulo(uint64_t iterations) {
  printf("Benchmarking Mersenne prime modulo...\n");
  double start = get_time_ms();
  uint64_t result = 0;

  for (uint64_t i = 0; i < iterations; i++) {
    result += i % MERSENNE_PRIME;
  }

  benchmark_run_t run = {get_time_ms() - start, result};
  return run;
}

benchmark_results_t
run_modulo_vs_bitshift_benchmark(uint64_t iterations) {
  benchmark_results_t results = {0};

  printf("Running modulo vs bit-shift benchmark with %llu iterations...\n", iterations);
  printf("Comparing four methods:\n");
  printf("  1. Non-prime modulo:     i %% ((1 << 32) - 6)\n");
  printf("  2. Power of two bitmask: i & ((1 << 32) - 1)\n");
  printf("  3. Mersenne prime shift: hash_bin_index(i, 32)\n");
  printf("  4. Mersenne prime modulo: i %% ((1 << 32) - 1)\n");
  printf("\n");

  benchmark_run_t run1 = benchmark_non_prime_modulo(iterations);
  benchmark_run_t run2 = benchmark_power_of_two_bitmask(iterations);
  benchmark_run_t run3 = benchmark_mersenne_prime_bitshift(iterations);
  benchmark_run_t run4 = benchmark_mersenne_modulo(iterations);

  results.non_prime_modulo_ms = run1.time_ms;
  results.power_of_two_bitmask_ms = run2.time_ms;
  results.mersenne_prime_bitshift_ms = run3.time_ms;
  results.mersenne_prime_modulo_ms = run4.time_ms;

  // Use results to prevent optimization
  uint64_t total = run1.result + run2.result + run3.result + run4.result;
  if (total == 0) printf("Unexpected: all results were zero\n");

  // Print results
  printf("\n");
  printf("╔════════════════════════════════════════════════════════════════╗\n");
  printf("║              MODULO vs BIT-SHIFT BENCHMARK RESULTS             ║\n");
  printf("╚════════════════════════════════════════════════════════════════╝\n");

  printf("\n=== Method Comparison ===\n");
  printf("  Non-prime modulo ((1 << 32) - 6):     %.3f ms\n", results.non_prime_modulo_ms);
  printf("  Power of two bitmask ((1 << 32) - 1): %.3f ms\n", results.power_of_two_bitmask_ms);
  printf("  Mersenne prime bitshift:              %.3f ms\n", results.mersenne_prime_bitshift_ms);
  printf("  Mersenne prime modulo:                %.3f ms\n", results.mersenne_prime_modulo_ms);

  // Find fastest and calculate speedups
  double fastest = results.non_prime_modulo_ms;
  const char* fastest_name = "Non-prime modulo";

  if (results.power_of_two_bitmask_ms < fastest) {
    fastest = results.power_of_two_bitmask_ms;
    fastest_name = "Power of two bitmask";
  }
  if (results.mersenne_prime_bitshift_ms < fastest) {
    fastest = results.mersenne_prime_bitshift_ms;
    fastest_name = "Mersenne prime bitshift";
  }
  if (results.mersenne_prime_modulo_ms < fastest) {
    fastest = results.mersenne_prime_modulo_ms;
    fastest_name = "Mersenne prime modulo";
  }

  printf("\n=== Performance Analysis ===\n");
  printf("  Fastest method: %s (%.3f ms)\n", fastest_name, fastest);
  printf("\n  Relative performance:\n");
  printf("    Non-prime modulo:        %.2fx slower than fastest\n", results.non_prime_modulo_ms / fastest);
  printf("    Power of two bitmask:    %.2fx slower than fastest\n", results.power_of_two_bitmask_ms / fastest);
  printf("    Mersenne prime bitshift: %.2fx slower than fastest\n", results.mersenne_prime_bitshift_ms / fastest);
  printf("    Mersenne prime modulo:   %.2fx slower than fastest\n", results.mersenne_prime_modulo_ms / fastest);

  printf("\n=== SUMMARY ===\n");
  printf("Power of 2 bitmask:\n");
  printf("  - Uses: i & (2^n - 1) instead of i %% 2^n\n");
  printf("  - Simple AND operation, very fast\n");
  printf("\nMersenne prime bitshift:\n");
  printf("  - Uses: hash_bin_index for i %% (2^n - 1)\n");
  printf("  - Formula: y = (x >> n) + (x & (2^n - 1)); return (y > 2^n - 1) ? y - (2^n - 1) : y\n");
  printf("  - More complex but avoids expensive modulo\n");
  printf("\nMersenne prime modulo:\n");
  printf("  - Uses: i %% (2^n - 1)\n");
  printf("  - Standard modulo with Mersenne prime\n");
  printf("\nNon-prime modulo:\n");
  printf("  - Uses: standard %% operator\n");
  printf("  - No bit-shift optimization available\n");

  return results;
}

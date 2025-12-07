#ifndef MODULO_VS_BITSHIFT_BENCHMARK_H
#define MODULO_VS_BITSHIFT_BENCHMARK_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

// Test values using bit-shift notation:
// 1. Non-prime close to 2^32: (1 << 32) - 6
// 2. Perfect power of 2: 1 << 32
// 3. Mersenne prime: (1 << 32) - 1

#define NON_PRIME_SIZE ((1ULL << 32) - 6)  // 2^32 - 6 (not prime)
#define POWER_OF_TWO_SIZE (1ULL << 32)     // 2^32
#define POWER_OF_TWO_SHIFT 32
#define MERSENNE_PRIME ((1ULL << 32) - 1)  // 2^32 - 1

// Default number of iterations for the benchmark loop
#define DEFAULT_ITERATIONS 100000000

typedef struct {
  double time_ms;
  uint64_t result;
} benchmark_run_t;

typedef struct {
  double non_prime_modulo_ms;
  double power_of_two_bitmask_ms;
  double mersenne_prime_bitshift_ms;
  double mersenne_prime_modulo_ms;
} benchmark_results_t;

double
get_time_ms(void);

benchmark_run_t
benchmark_non_prime_modulo(uint64_t iterations);

benchmark_run_t
benchmark_power_of_two_bitmask(uint64_t iterations);

benchmark_run_t
benchmark_mersenne_prime_bitshift(uint64_t iterations);

benchmark_run_t
benchmark_mersenne_modulo(uint64_t iterations);

benchmark_results_t
run_modulo_vs_bitshift_benchmark(uint64_t iterations);

#endif  // MODULO_VS_BITSHIFT_BENCHMARK_H

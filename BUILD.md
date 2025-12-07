# Building the Project

## Using CMake (Recommended)

### First-time setup:
```bash
# Create a build directory
mkdir build
cd build

# Configure the project
cmake ..

# Build the executable
cmake --build .
```

### Run the benchmark:
```bash
# From the build directory
cmake --build . --target run_benchmark
```

### View the results:
```bash
# From the build directory
cmake --build . --target view_results
```

### Clean build:
```bash
# Remove the build directory and start fresh
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
```

## Manual Compilation

If you prefer to compile manually without CMake:

```bash
# From the src directory
cd src
clang -O2 main.c hash_table.c ../benchmarks/modulo_vs_bitshift_benchmark.c -o hash_table

# Run and save results
./hash_table > ../benchmarks/result.txt
```

## Project Structure

```
joys-of-hashing/
├── src/
│   ├── main.c                           # Entry point
│   ├── hash_table.c                     # Hash table implementation
│   ├── hash_table.h
│   ├── hash_table_with_free_bit.c
│   ├── hash_table_with_free_bit.h
│   └── hash_table_helper.h
├── benchmarks/
│   ├── modulo_vs_bitshift_benchmark.c   # Benchmark implementation
│   ├── modulo_vs_bitshift_benchmark.h
│   └── result.txt                       # Benchmark output
└── CMakeLists.txt                       # CMake configuration
```

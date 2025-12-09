# This was written by claude :shrug:, same for the build.md file.
cmake_minimum_required(VERSION 3.15)
project(joys-of-hashing C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Set aggressive optimization flags
set(CMAKE_C_FLAGS_RELEASE "-O3 -march=native -mtune=native")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -mtune=native")

# Define source files
set(HASH_TABLE_SOURCES
    src/hash_table.c
    src/hash_table_with_free_bit.c
)

set(BENCHMARK_SOURCES
    benchmarks/modulo_vs_bitshift_benchmark.c
)

# Main executable
add_executable(hash_table
    src/main.c
    ${HASH_TABLE_SOURCES}
    ${BENCHMARK_SOURCES}
)

# Include directories
target_include_directories(hash_table PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/benchmarks
)

# Set output directory for the executable
set_target_properties(hash_table PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src
)

# Custom target to run the benchmark and save results
add_custom_target(run_benchmark
    COMMAND ${CMAKE_SOURCE_DIR}/src/hash_table > ${CMAKE_SOURCE_DIR}/benchmarks/result.txt
    DEPENDS hash_table
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running hash table benchmark and saving results to benchmarks/result.txt"
)

# Custom target to view results
add_custom_target(view_results
    COMMAND cat ${CMAKE_SOURCE_DIR}/benchmarks/result.txt
    COMMENT "Displaying benchmark results"
)

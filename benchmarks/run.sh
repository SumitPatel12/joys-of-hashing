# Configuration
MERSENNE_POWER=19       # 2^19 - 1 = 524,287 slots
NUM_ITEMS=400000        # Load factor ~0.76
ITERATIONS=10           
OUT_FILE="benchmark_results_lookup.csv"

# Compile Chaining
echo "Compiling Chaining Benchmark..."
clang -O2 -DWITH_METRICS -DUSE_CHAINING -o bench_chaining main.c ../src/hash_table.c

if [ $? -ne 0 ]; then
    echo "Compilation of Chaining failed!"
    exit 1
fi

# Compile Open Addressing
echo "Compiling Open Addressing Benchmark..."
clang -O2 -DWITH_METRICS -o bench_oa main.c ../src/open_addressing.c

if [ $? -ne 0 ]; then
    echo "Compilation of Open Addressing failed!"
    exit 1
fi

# Initialize CSV
echo "Type,InsertTime,LookupTime,Count,Collisions" > $OUT_FILE

echo "Running Benchmarks (Size 2^$MERSENNE_POWER - 1)..."

# Run Loops
for ((i=1; i<=ITERATIONS; i++))
do
    SEED=$((12345 + i))
    
    # Run Chaining
    # Capturing only the line starting with BENCH
    ./bench_chaining $SEED $NUM_ITEMS $MERSENNE_POWER | grep "^BENCH" | cut -d',' -f2- >> $OUT_FILE
    
    # Run Open Addressing
    ./bench_oa $SEED $NUM_ITEMS $MERSENNE_POWER | grep "^BENCH" | cut -d',' -f2- >> $OUT_FILE
    
    echo -n "."
done
echo ""
echo "Done. Results in $OUT_FILE"

# Analyze Results (Simple average using awk)
echo ""
echo "=== Summary ==="
awk -F, '
{
    if ($1 == "Chaining") {
        c_inst += $2; c_look += $3; c_coll += $5; c_count++;
    } else if ($1 == "Open Addressing") {
        o_inst += $2; o_look += $3; o_coll += $5; o_count++;
    }
}
END {
    if (c_count > 0) {
        printf "Chaining (Avg of %d runs):\n", c_count;
        printf "  Insert Time: %.6f s\n", c_inst / c_count;
        printf "  Lookup Time: %.6f s\n", c_look / c_count;
        printf "  Collisions:  %.2f\n", c_coll / c_count;
    }
    if (o_count > 0) {
        printf "Open Addressing (Avg of %d runs):\n", o_count;
        printf "  Insert Time: %.6f s\n", o_inst / o_count;
        printf "  Lookup Time: %.6f s\n", o_look / o_count;
        printf "  Collisions:  %.2f\n", o_coll / o_count;
    }
}' $OUT_FILE

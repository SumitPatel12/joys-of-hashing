# Benchmark Comparison: Chaining vs Open Addressing

We have implemented metric tracking and ran benchmarks to compare the performance of Chaining (Linked Lists) versus Open Addressing (Linear Probing) using Mersenne Prime table sizes ($2^{18}-1 \approx 262,143$).

## Results (Average of 10 runs)

| Metric | Chaining | Open Addressing |
| :--- | :--- | :--- |
| **Time (seconds)** | 0.004310 | 0.002572 |
| **Collisions** | 60,131 | 322,146 |
| **Items Inserted** | 200,000 | 200,000 |


> [!NOTE]
> **Load Factor**: $\approx 0.76$ ($200,000 / 262,143$).

## Large Table Results (Mersenne Power 19)

We also ran benchmarks with a larger table size ($2^{19}-1 = 524,287$) and 400,000 items (Load Factor $\approx 0.76$).

| Metric | Chaining | Open Addressing |
| :--- | :--- | :--- |
| **Time (seconds)** | 0.009018 | 0.004645 |
| **Collisions** | 120,116 | 645,419 |

The trend holds: Open Addressing is still ~1.9x faster than Chaining, despite the higher collision (probe) count.

## Huge Table Results (Mersenne Power 31)

We tested with a massive table size of $2^{31}-1 \approx 2.14$ Billion slots.
- **Memory Usage**: $\approx 16$ GB.
- **Items**: 10 Million (Load Factor $\approx 0.005$).
- **Goal**: Test performance effectively dominated by memory caching behavior (TLB/DRAM latency).

| Metric | Chaining | Open Addressing |
| :--- | :--- | :--- |
| **Time (seconds)** | 25.29 | 26.68 |
| **Collisions** | 11,624 | 11,688 |

### Observation
At this scale, the performance advantage of Open Addressing vanishes. Both methods perform similarly, with Chaining being marginally faster.
- **Reasoning**: Performance is dominated by **TLB misses** and **Main Memory Latency** due to random access over a 16GB region. The CPU cache (L3) is ineffective.
- The slight overhead of Open Addressing (larger struct size or probing logic) might make it slightly slower, but both are effectively bound by the time to fetch a random memory page.

## Lookup Time Results (Mersenne Power 19)

We reverted to the table size $2^{19}-1$ (524,287 slots) with 400,000 items to specifically benchmark **Lookup** performance (checking existence of all inserted items).

| Metric | Chaining | Open Addressing |
| :--- | :--- | :--- |
| **Insert Time (s)** | 0.008129 | 0.004984 |
| **Lookup Time (s)** | 0.002972 | 0.004495 |
| **Insert Collisions** | 120,116 | 645,419 |

### Observation
**Chaining is faster at Lookups (~1.5x speedup over OA).**

### Analysis
- **Open Addressing**: While insertion is fast (cpu cache friendly linear write), lookup suffers from **clustering**.
    - With $\alpha=0.76$, many items are not in their ideal slot.
    - Resolving a lookup requires probing multiple slots (Average collisions/probes is much higher).
    - Each probe is a calculation + memory check.
- **Chaining**:
    - $\alpha=0.76$ means most bins have 0 or 1 item.
    - $P(\text{empty}) = e^{-0.76} \approx 46\%$.
    - $P(\text{1 item}) \approx 35\%$.
    - Most lookups are $O(1)$ immediate hits (head of list).
    - Traversing a short linked list (length ~1.5 avg for non-empty) involves fewer total comparisons than the probe sequence in OA at this load factor.

## Conclusion

### Performance
- **Open Addressing** is significantly **faster** (~1.7x speedup). This is expected because:
    - It avoids `malloc` overhead for every insertion.
    - It has better cache locality (linear scan of array vs chasing pointers).
- **Chaining** is slower primarily due to dynamic memory allocation (`malloc`).

### Collisions
- **Chaining Collisions** (60k) count "non-empty bin encounters". With random data, many bins remain empty or have 1 item.
- **Open Addressing Collisions** (322k) count "occupied slot encounters" during probing. Since clusters form (Linear Probing), a single insertion can trigger multiple collisions (probes).
    - At $\alpha=0.76$, clustering becomes significant, leading to higher probe counts.

## Implementation Details

### Metrics
We added a `WITH_METRICS` macro to `hash_table` and `open_addressing`.
- **Chaining**: Counts collision if `bin != NULL` before insert.
- **Open Addressing**: Counts collision for every occupied slot seen during linear probing.

### Benchmarks
- `benchmarks/run.sh` runs both implementations 10 times with different random seeds.
- Both implementations use **Mersenne Prime** sizing to ensure fair comparison logic (`hash_table_helper.h`).

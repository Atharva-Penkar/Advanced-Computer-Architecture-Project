# Advanced Computer Architecture Project

**Authors:**  
Aditya Dhananjay Singh (22cs02001),
Kumar Snehal (22cs02009),
Atharva Atul Penkar (22cs02011)

**Report:**  
Refer to the detailed project report [ACA_Lab_Project_Report.pdf](./ACA_Lab_Project_Report.pdf)

---

## Project Overview

This project enhances the SimpleScalar cache simulator by implementing:

- A **Least Frequently Used (LFU)** cache replacement policy.
- Multiple **prefetcher policies**: *None*, *Next-Line*, and *Stride Prefetching.*

These extensions allow robust evaluation of cache behavior under diverse replacement and prefetch strategies.

---

## Changes Made and Locations

### 1. LFU Cache Policy
- **Location:** `cache.c`, `cache.h`  
- **Details:**  
  - Each cache block gains a `freq_count` tracking number of accesses.  
  - LFU evicts the block with the lowest frequency on replacement.  
  - Block frequency increments on every hit or access.  
  - Replacement logic updated to scan and select minimum frequency block during eviction.

### 2. Prefetcher Support and Integration
- **Location:** `cache.c`, `cache.h`, simulator config  
- **Details:**  
  - Cache struct extended with `enum prefetcher_policy prefetcher` and stride table allocation for stride prefetcher.  
  - `cache_create()` initializes stride prefetcher structures when enabled.  
  - Command-line option (`-prefetcher`) added for runtime prefetcher selection.  
  - Prefetchers invoked on cache misses and slow hits within `cache_access()`.

---

## How the Prefetchers Work

### Next-Line Prefetcher
- **When:** On cache misses and slow hits. No prefetch on repeated fast hits.  
- **Logic:** Always prefetch the immediately next consecutive cache block after the current one.  
- **Use Case:** Sequential memory scans like array traversals.  
- **Limitation:** Prefetches may waste bandwidth if access pattern is irregular.

### Stride Prefetcher
- **When:** On misses and slow hits, but only prefetches after learning a stable stride.  
- **Logic:**  
  - Maintains a stride table indexed by XOR-folded block addresses to reduce collisions:
    ```
    index = ((block_addr / block_size) ^ ((block_addr / block_size) >> 4)) % stride_table_size;
    ```
  - Tracks last accessed block, stride, and confidence per table entry.  
  - If current stride matches stored stride, increases confidence (max 3).  
  - On stride change, resets confidence to 1 for quick relearning.  
  - Once confidence ≥ 1, predicts next block address as current + stride and issues prefetch.  
- **Use Case:** Regular but non-sequential patterns like strided matrix or scientific data access.  
- **Benefit:** Adapts and prefetches more effectively in patterned, non-consecutive accesses.

---

## Prefetch Request Handling
- Prefetches are non-blocking, issued asynchronously to overlap with normal execution.  
- Cache misses and evictions proceed normally, keeping prefetch transparent to CPU pipeline.  
- Prefetcher avoids dependence on program counter by hashing block addresses with XOR folding for indexing.

---

## Why These Enhancements Matter

- **LFU** protects frequently used blocks better than recency-based policies, beneficial in workloads with hotspots.  
- **Next-Line** exploits strong spatial locality in sequential workloads simply and efficiently.  
- **Stride** intelligently prefetches non-sequential but regular accesses, improving cache performance in more complex applications.  
- **XOR folding** reduces stride table collisions without adding PC tracking complexity.  
- **Confidence mechanism** avoids premature or incorrect prefetches, increasing accuracy and reducing useless memory traffic.

---

## Practical Summary

| Feature           | Description                                     | Ideal Use Case               | Limitation                    |
|-------------------|------------------------------------------------|-----------------------------|-------------------------------|
| LFU Replacement   | Evicts blocks used least frequently             | Hotspot-heavy workloads      | Requires tracking frequency     |
| Next-Line Prefetch| Prefetches next consecutive block               | Sequential memory scans      | Inefficient for irregular access|
| Stride Prefetch   | Learns stride pattern, prefetches accordingly   | Strided/patterned memory ops | Needs stable repetitive strides  |

---

## Usage

- Set replacement: `-repl=lfu` for LFU, else `l`, `f`, or `r` for others.  
- Set prefetcher: `-prefetcher=none`, `-prefetcher=next_line`, or `-prefetcher=stride`.  
- Execute benchmarks and analyze cache hit rates to assess impact.

---

## How to Run

- Use the provided scripts:
  - `runAll.sh` to automate runs across benchmarks and cache configs.  
  - `extract_data.sh` to gather and prepare results.  
  - `plotdata.py` to visualize benchmark outcomes.  
- Make scripts executable with:  
```
chmod +x runAll.sh
./runAll.sh
```
---

# Cache Benchmark Suite Readme

This document summarizes the eight primary cache workloads used for testing replacement policies, prefetcher effectiveness, and geometry sensitivity.

---

## 1. Multi-Stream Chase Benchmark (Prefetcher Stress Test)

This benchmark forces a severe conflict across independent, linear streams to test prefetchers under heavy capacity pressure.

**Purpose:** To generate predictable memory accesses that are forced out of the cache quickly.

| Component | Mechanism | Stress Test Focus |
| :--- | :--- | :--- |
| **Data Footprint** | Massive (512 KB+) | Ensures data **exceeds cache capacity**, guaranteeing continuous eviction. |
| **Outer Loop (8 Streams)** | Jumps between 8 independent memory regions. | Creates severe **Capacity Conflict** by flushing blocks needed just moments ago. |
| **Inner Loop (Sequential)** | Linear access within each stream. | Provides **perfect spatial locality** (fixed stride) for $\text{Next-Line}$ and $\text{Stride}$ prefetchers to successfully predict and hide latency. |

The expected result is a clear performance hierarchy where $\text{Next-Line}$ and $\text{Stride}$ policies significantly outperform the $\text{None}$ policy.

---

## 2. Depth-First Search (DFS) Pointer Chase Benchmark

This benchmark is designed to create cache instability by simulating highly scattered data accesses in memory.

**Purpose:** To generate accesses that are mathematically **unpredictable** due to heap fragmentation.

| Component | Mechanism | Cache Access Pattern |
| :--- | :--- | :--- |
| **Heap Fragmentation** | Uses dummy allocations to scatter Node structs across memory. | Intentionally destroys **Spatial Locality**. |
| **Traversal Workload** | Executes a recursive DFS following arbitrary pointers. | The memory access sequence is **unpredictable**. |

**Expected Outcome:** Prefetchers should show **zero benefit** over the $\text{None}$ policy.

---

## 3. Graph Traversal (BFS) Benchmark

This benchmark simulates searching a large, dense graph using a Breadth-First Search (BFS) algorithm.

**Purpose:** To stress the cache with alternating access patterns: highly sequential (queue) and highly random (graph node access).

| Component | Mechanism | Cache Access Pattern |
| :--- | :--- | :--- |
| **Node Access (`curr->neighbors[i]`)** | **Pointer Chasing** | Memory addresses of neighboring nodes are widely scattered, generating unpredictable **Compulsory/Capacity Misses**. |
| **Queue Operations** | **High Sequential Access** | The queue provides high locality for control data, contrasting with the graph data access. |

**Expected Outcome:** LRU/LFU policies should perform better than FIFO/Random due to better management of active neighbor blocks.

---

## 4. Hot/Cold Region Access Benchmark

This synthetic workload is designed to demonstrate LFU's superiority by explicitly punishing time-based policies (like LRU).

**Purpose:** To access a small, useful "hot" region much more frequently than a large "cold" region used purely for flushing the cache.

| Component | Mechanism | Stress Test Focus |
| :--- | :--- | :--- |
| **Hot Region (80% of Accesses)** | Small region accessed constantly. | Blocks are always high-frequency, but can become 'Least Recently Used' very quickly. |
| **Cold Region (20% of Accesses)** | Large region accessed randomly within itself. | Sweeps out Hot Blocks, forcing LRU to evict useful data. |

**Expected Outcome:** LFU should show **significantly higher hit rates** than LRU, FIFO, and Random.

---

## 5. Random Permutation Benchmark

This benchmark simulates a highly fragmented, unpredictable access pattern that minimizes locality across the data set.

**Purpose:** To establish a baseline worst-case scenario for hit rates and measure the intrinsic overhead of each replacement policy.

| Component | Mechanism | Cache Access Pattern |
| :--- | :--- | :--- |
| **Access Indices Array** | **Pre-shuffled Pointers** | The data access order is arbitrary and unpredictable across the entire memory range. |
| **Overall Stress** | **Randomness Dominates** | Destroys both temporal and spatial locality; differences between predictive and non-predictive policies should be minimal. |

**Expected Outcome:** Hit rates for all policies should be low and clustered closely together.

---

## 6. Stair-Step Benchmark

This benchmark simulates an optimized memory access pattern typical in dense matrix operations and image processing.

**Purpose:** To create bursts of intense temporal locality followed by severe capacity conflicts.

| Component | Mechanism | Cache Access Pattern |
| :--- | :--- | :--- |
| **Matrix Tiling** | Accesses small, contiguous blocks ($16 \times 16$) before jumping to the next block. | High spatial locality within the tile; abrupt capacity miss when jumping to the next tile. |
| **Overall Stress** | **Recency Test** | Predictable access allows LRU to perform exceptionally well, provided the tile size fits the cache. |

**Expected Outcome:** LRU/LFU should significantly outperform FIFO/Random.

---

## 7. Fixed Stride Benchmark

This simple linear benchmark verifies that the prefetcher is functional and accurate at a specific, large stride.

**Purpose:** To test the reliability and latency-hiding ability of the Stride Prefetcher.

| Component | Mechanism | Cache Access Pattern |
| :--- | :--- | :--- |
| **Array Access** | Accesses a large array with a fixed, non-unit stride (e.g., $+256$ bytes). | Perfect mathematical predictability. |
| **Overall Stress** | **Stride Predictor Test** | Forces a miss on every access, but the address of the next miss is always known. |

**Expected Outcome:** $\text{Stride}$ prefetcher must perform significantly better than $\text{Next-Line}$ (which will fail due to the large stride) and $\text{None}$.

---

## 8. Temporal Decay Access Benchmark

This benchmark simulates a shifting working set where old, frequently used data becomes obsolete quickly.

**Purpose:** To measure how efficiently replacement policies discard old, high-utility data in favor of new data.

| Component | Mechanism | Cache Access Pattern |
| :--- | :--- | :--- |
| **Phased Access** | Access is concentrated in a "working set" region for a set period, then the working set permanently shifts to an adjacent region. | Creates blocks with high but obsolete frequency counts. |
| **Overall Stress** | **Working Set Adaptation** | Challenges LFU to quickly evict blocks with historically high counts. LRU typically performs well here. |

**Expected Outcome:** LRU should perform well, while a naïve LFU implementation may struggle due to its reliance on outdated historical frequency data.

</br>
</br>
</br>
</br>
</br>
</br>
</br>
</br>

![alt text](plot_1_prefetcher_comparison_normalized.png)

We  have  taken  the  average  of  DL1  hit  rates  across  all  the  benchmarks  and  replacement 
policies. 
The graph indicates that the next line prefetcher delivers the best performance. This superior 
result  is  attributed  to  the  spatial  locality  present  in  the arrays within most of our benchmark 
codes, which the next line prefetcher effectively exploits.  
Following this, the prefetcher with no specific settings shows intermediate performance. 
Conversely,  the  stride  prefetcher  performs  poorly  because  the  benchmarks  lack  the  large 
strides required for it to be effective.

![alt text](plot_2_replacement_vs_prefetcher.png)

LRU (Least Recently Used) LRU provides the best prediction by perfectly exploiting temporal 
locality. It reliably evicts the block that has been unused for the longest time. 
LFU (Least Frequently Used) LFU tracks cumulative access count, protecting the blocks used 
 
most over the long term. It can fail if a critical, non-repeating instruction block is evicted because 
its count is low. 
FIFO (First-In, First-Out) FIFO evicts the oldest block in the cache, ignoring how recently or 
often it has been used. This complete disregard for temporal locality leads to poor performance 
in cyclic workloads. 
Random  Random  replacement  offers  no  predictive  ability  and  serves  as  the  performance 
baseline. It will perform arbitrary evictions, resulting in the highest unnecessary miss rate. 
LRU replacement policy performs the best out of all.

![alt text](plot_3_benchmark_breakdown_fixed.png)

We are using the Data Cache since we know that the Instruction will naturally be similar due to 
the codes being  stored together. 
There  are  a  lot  of  metrics  here  which  cover  all  the  replacement  policies,  and  prefetching 
algorithms with the benchmarks we found or created and used. 
We will be covering specific data points to perform our analysis. 
 
![alt text](plot_4_assoc_repl_benchmark_comparison.png)

1-way (Direct Mapped): Lowest performance due to maximum Conflict Misses (blocks always 
fight for the same slot). Check performance decrease in . 
2-way & 4-way: Significantly reduces conflict misses, with 2-way giving the largest jump in hit 
rate. 4-way offers minimal extra gain but approaches ideal mapping by reducing competition to 
only Capacity Misses. 
 
---

## Conclusion

The project delivers advanced cache replacement politices and dynamic prefetching in SimpleScalar, supporting diverse workload simulations. LFU improves cache block retention based on access frequency, while the stride prefetcher dynamically learns and exploits non-linear but regular memory access patterns. These improvements enable thorough evaluation and tuning of caching strategies for modern architectures.

For further details, please see the full project report: [ACA_Lab_Project_Report.pdf](./ACA_Lab_Project_Report.pdf)

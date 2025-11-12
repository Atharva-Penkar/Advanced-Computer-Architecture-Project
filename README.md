# LFU Cache and Prefetcher Enhancements – Comprehensive Project Overview

## Project Overview
This project enhances the SimpleScalar cache simulator by implementing:
- A **Least Frequently Used (LFU)** cache replacement policy.
- Multiple **prefetcher policies**, specifically **none**, **next-line**, and **stride prefetching**.

These additions provide the ability to simulate and analyze advanced cache behaviors for diverse workloads.

---

## Changes Made and Where

### 1. LFU Cache Policy
- **File:** `cache.c`, `cache.h`  
- **What:**  
  - Added a `freq_count` field to each cache block to track access frequency.  
  - In the LFU policy, on cache replacement, the block with the lowest `freq_count` is evicted.  
  - Modified replacement logic in `cache_access()` to scan blocks for minimum frequency.  
  - Increment `freq_count` on every hit or access to reflect block popularity.

### 2. Prefetcher Support and Integration
- **File:** `cache.c`, `cache.h`, simulator config  
- **What:**  
  - Extended the cache structure to include `enum prefetcher_policy prefetcher` and a dynamically allocated stride table.  
  - `cache_create()` allocates stride table entries if stride prefetcher enabled.  
  - Added command-line flag (`-prefetcher`) for selecting policy dynamically at runtime.  
  - Prefetcher logic invoked on cache misses and slow hits during `cache_access()`.

---

## How Each Prefetcher Works

### Next-Line Prefetcher
- **When:** Triggered on all misses and slow hits (new block accesses), not on fast hits (repeated block access).  
- **Logic:** Always prefetches the immediately next sequential block (current block address + block size).  
- **Use Case:** Ideal for workloads with sequential memory access patterns like linear array traversals.  
- **Limitations:** Inefficient for sparse or irregular access patterns and may waste memory bandwidth.

### Stride Prefetcher
- **When:** Triggered on misses and slow hits, but only issues prefetches after learning a stable stride pattern.  
- **Logic:**  
  - Maintains a stride table with entries storing last accessed block address, detected stride, and confidence.  
  - Indexing of stride table uses XOR folding of block address bits to reduce collisions:
    ```
    index = ((block_addr / block_size) ^ ((block_addr / block_size) >> 4)) % stride_table_size
    ```
  - On each new access:
    - Compute the stride as the difference from the last block address.  
    - If stride matches stored stride, increase confidence (max 3).  
    - If stride differs, update stride and reset confidence to 1 (enables rapid relearning).  
  - Once confidence ≥ 1, prefetch the block at current block address + stride.  
- **Use Case:** Best suited for workloads with regular, strided memory access patterns (e.g., matrix columns, strided scientific data).  
- **Benefits:** Adapts to non-sequential predictable patterns and issues efficient prefetches, improving cache hit rates.

---

## Prefetch Request Handling
- Prefetch requests are **non-blocking** and overlap with normal execution to hide latency without stalling the CPU.
- Miss handling and block replacements proceed as normal, ensuring prefetching is transparent to the processor pipeline.
- The prefetcher runs without requiring the program counter (PC) by using address-based XOR folded hashing for indexing.

---

## Why These Changes Matter

- **LFU Policy:** Evicts rarely accessed blocks, improving cache efficiency for programs with hotspots or uneven data reuse that LRU or FIFO may not capture.
- **Next-Line Prefetcher:** Simple, predictable, works well for sequential patterns.
- **Stride Prefetcher:** Captures more complex, regular access strides, leading to better performance on workloads with recurring access jumps.
- **XOR Folding:** Balances indexing simplicity and effectiveness, minimizing aliasing without complicated PC tracking.
- **Confidence Mechanism:** Avoids premature prefetching, reducing useless memory traffic and increasing accuracy.

---

## Practical Summary

| Feature           | What It Does                               | Ideal For                          | Limitation                       |
|-------------------|-------------------------------------------|----------------------------------|---------------------------------|
| LFU Replacement   | Evicts least frequently used block        | Workloads with hotspots          | Slight overhead to track freq   |
| Next-Line Prefetch| Prefetches next sequential cache block    | Strictly sequential data         | Inefficient for irregular pattern|
| Stride Prefetch   | Detects and prefetches consistent strides | Strided and patterned access     | Needs repeated pattern & stable confidence |

---

## Usage

- Set replacement policy: `:q` instead of `:l/r/f` in cache configuration for LFU replacement.
- Set prefetcher policy: `-prefetcher=none`, `-prefetcher=next_line`, or `-prefetcher=stride`.
- Run simulations to analyze performance impact using different configurations.

---

## To Run
You may use the existing shell script `runAll.sh` in order for the code to run:
- `run_benchmarks.sh`: runs all benchmarks in ./benchmarks with all the cache configurations in ./cache_configs with all 3 prefetcher options.
- `extract_data.sh`: goes over all the results stored in ./results directory plots the dl1 and il1 hit rates in `benchmarks_data.csv`.
- `plotdata.py`: reads data from `ACAprojectdata.csv` which is manually cleaned data from `benchmarks_data.csv` and makes plots.
Steps:
- chmod +x runAll.sh
- ./runAll.sh

## Conclusion

This project enhances SimpleScalar with powerful cache and prefetcher options. LFU improves replacement decisions for non-uniform access, while stride prefetching adapts to pattern-based access, both extending simulator capability to realistically model advanced processor memory systems and optimize performance for diverse workloads.

# Advanced Computer Architecture Project

**Authors:**  
Aditya Dhananjay Singh (22cs02001), Kumer Snehal (22cs02009), Aharva Atul Penkar (22cs02011)

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

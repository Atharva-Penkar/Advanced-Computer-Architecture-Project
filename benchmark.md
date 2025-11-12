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
# Prefetcher Implementation Overview

## Files Modified and Changes Made

- **cache.h**
  - Added `enum prefetcher_policy` with values `PREF_NONE`, `PREF_NEXT_LINE`, and `PREF_STRIDE`.
  - Extended `struct cache_t` to hold:
    - `enum prefetcher_policy prefetcher` to select the current prefetcher.
    - Data structures for stride prefetcher: a `stride_table` array and its size.
  - Updated the `cache_create()` function signature to accept `prefetcher` as a parameter.

- **cache.c**
  - Implemented initialization of the `prefetcher` field and stride table allocation in `cache_create()`.
  - Added a helper function `cache_prefetch()` to issue a non-blocking prefetch access to the cache.
  - Extended `cache_access()` to invoke the selected prefetcher after each cache access:
    - **Next-Line Prefetcher:** Prefetches the immediately following cache line (address + block size).
    - **Stride Prefetcher:** Maintains a small table of recent addresses to detect access strides and prefetches the next address based on detected stride patterns.
  - Updated cache block and set handling to support the new logic safely.

- **sim-cache.c**
  - Added a new command-line option `-prefetcher` to select the prefetcher at runtime.
  - Parsed the prefetcher string from command line and passed the corresponding enum value into every `cache_create()` call.
  - Declared and used a global `prefetcher` variable, ensuring correct visibility across the file.
  
## Why These Changes Were Made

- To provide flexible, runtime-selectable cache prefetching policies within the SimpleScalar sim-cache simulator without hardcoding a single behavior.
- The new prefetcher architecture allows easy experimentation and comparison of distinct prefetching strategies for research or educational purposes.
- Extending `cache_t` with prefetcher state supports stride detection in hardware-simulated cache.
- Adding `-prefetcher` CLI option enables users to select among `none`, `next_line`, or `stride` prefetching without rebuilding the simulator.
  
## Impact of the Changes

- **Behavioral**:  
  - When enabled, the next-line prefetcher fetches the cache block immediately after the current access, attempting to reduce miss latency for sequential accesses.  
  - The stride prefetcher dynamically detects regular strides in access patterns (including non-consecutive strides) and prefetches likely future blocks, improving cache hit rates for certain workloads.  
  - When `none` is selected, no prefetching occurs, allowing baseline comparisons.

- **Performance**:  
  - Prefetching reduces miss penalties when predictions are accurate but can increase bandwidth usage and cache pollution if inaccurate.  
  - Stride prefetching is more adaptive and potentially more effective than simple next-line prefetching but requires additional bookkeeping.

## Differences Between Prefetcher Policies

| Prefetcher       | Mechanism                                                | When Effective                                   | Complexity              |
|------------------|----------------------------------------------------------|-------------------------------------------------|-------------------------|
| `none`           | No prefetching                                           | Baseline; no overhead                            | None                    |
| `next_line`      | Always prefetches the next sequential cache block        | Sequential access patterns                       | Simple; no extra state  |
| `stride`         | Detects stride by comparing consecutive addresses; prefetches next predicted block based on stride | Works well for strided or patterned memory access, not limited to sequential | More complex; needs stride tracking table |



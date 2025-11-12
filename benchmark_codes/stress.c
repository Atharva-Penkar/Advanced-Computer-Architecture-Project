#include <stdio.h>

/* --- BENCHMARK PARAMETERS --- */
// Total working set size (MUST be >> 2 KB cache size)
// 100 blocks * 16 bytes/block * 4 ways = 6,400 bytes total, or ~3.1x cache size
#define TOTAL_BLOCKS 4096
#define BLOCK_SIZE 16
#define ARRAY_SIZE (TOTAL_BLOCKS * BLOCK_SIZE) // ~64 KB total working set

// Size of the hot region (in blocks). Must be small enough to fit in cache.
// 20 blocks * 16 bytes/block = 320 bytes (fits easily in 2KB cache)
#define HOT_BLOCKS 20

// Total number of accesses to run
#define NUM_ACCESSES 5000000

// Access Ratio:
#define HOT_ACCESS_RATIO 10  // 10% of accesses hit the Hot region
#define COLD_ACCESS_RATIO 90 // 90% of accesses flood the Cold region
/* ---------------------------- */

char data_array[ARRAY_SIZE];

void initialize_array() {
    // Variable declarations for C89
    long i;

    printf("Initializing data array (%lu bytes).\n", (long)ARRAY_SIZE);

    for (i = 0; i < ARRAY_SIZE; i++) {
        data_array[i] = 0;
    }
}

void hot_spot_flusher_benchmark() {
    // Variable declarations for C89
    long i;
    long index;
    long cold_offset;
    int sum;

    printf("Starting LFU Stress Test.\n");
    sum = 0;

    // Calculate the size of the cold region in memory addresses
    // Cold region starts after the hot region and fills the rest of the array
    cold_offset = HOT_BLOCKS * BLOCK_SIZE;

    for (i = 0; i < NUM_ACCESSES; i++) {

        // --- Access Pattern Logic ---
        if ((i % 100) < HOT_ACCESS_RATIO) {
            // 10% chance: Access the tiny HOT region (High Frequency)
            // Access is cyclical within the first HOT_BLOCKS * BLOCK_SIZE bytes
            index = (i * BLOCK_SIZE) % cold_offset;
        } else {
            // 90% chance: Access the large COLD region (Low Frequency / High Recency)
            // Access is sequential across the large cold region, acting as a FLUSHER.
            // This ensures blocks from the HOT region become the LRU candidates.
            index = cold_offset + ((i * BLOCK_SIZE) % (ARRAY_SIZE - cold_offset));
        }

        // Memory Access (Read and Write)
        data_array[index] = data_array[index] + 1;
        sum += data_array[index];
    }

    // Prevent compiler optimization of 'sum'
    if (sum == -1)
        printf("dummy: %d\n", sum);
}

int main() {
    // Variable declarations for C89
    int result;

    initialize_array();
    hot_spot_flusher_benchmark();

    printf("End of LFU Stress Test.\n");

    result = 0;
    return result;
}

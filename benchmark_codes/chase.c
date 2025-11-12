#include <stdio.h>

/* --- BENCHMARK PARAMETERS --- */
// Total number of independent memory streams.
// Set higher than cache associativity (e.g., 8 > 4 ways) to force capacity misses.
#define N_STREAMS 8

// Size of each data stream (in blocks, assuming 64-byte blocks).
// STREAM_SIZE * N_STREAMS * sizeof(int) MUST exceed L1 cache size (e.g., 512KB > 32KB L1)
#define STREAM_BLOCKS 16384

// Number of times to loop over the entire dataset for intensive testing
#define NUM_ITERATIONS 100
/* ---------------------------- */

// Array to hold the data for all streams
// This creates a large, contiguous block of memory.
int data_matrix[N_STREAMS][STREAM_BLOCKS];

void initialize_matrix() {
    int i, j;

    printf("Matrix initialization.\n");

    for (i = 0; i < N_STREAMS; i++) {
        for (j = 0; j < STREAM_BLOCKS; j++) {
            // Fill with arbitrary values
            data_matrix[i][j] = i * STREAM_BLOCKS + j;
        }
    }
}

void multi_chase_benchmark() {
    int i, j, stream_idx;
    int sum;

    printf("Starting Multi-Stream Chase access.\n");

    sum = 0;

    for (i = 0; i < NUM_ITERATIONS; i++) {
        // Iterate through all blocks in one stream before jumping to the next stream
        for (stream_idx = 0; stream_idx < N_STREAMS; stream_idx++) {
            // Sequential access within the current stream
            // The memory access pattern is perfectly sequential block-to-block.
            for (j = 0; j < STREAM_BLOCKS; j++) {

                // Memory Access: Read and Write
                // This forces sequential, fixed-stride access across a vast memory range.
                data_matrix[stream_idx][j] = data_matrix[stream_idx][j] + 1;
                sum += data_matrix[stream_idx][j];
            }
        }
    }
    // Prevent compiler optimization of 'sum'
    if (sum == -1)
        printf("dummy: %d\n", sum);
}

int main() {
    // Declarations for C89 compatibility MUST be at the beginning of the function
    int result;

    initialize_matrix();
    multi_chase_benchmark();

    printf("End of Multi-Stream Chase benchmark.\n");

    result = 0;
    return result;
}

#include <stdio.h>
#define TOTAL_ARRAY_SIZE 4096
#define HOT_REGION_SIZE 256 // e.g., 256 integers, 1KB (4 bytes/int)
#define NUM_ACCESSES 20000000

int access_array[TOTAL_ARRAY_SIZE];

void initialize_hot_cold() {
    printf("Initializing hot cold region access.\n");
    int i;
    for (i = 0; i < TOTAL_ARRAY_SIZE; i++) {
        access_array[i] = 0;
    }
}

void hot_cold_benchmark() {
    printf("Running...\n");
    int i, index;
    int sum = 0;
    for (i = 0; i < NUM_ACCESSES; i++) {
        // 80% of accesses target the small HOT region (0 to HOT_REGION_SIZE-1)
        if ((i % 10) < 8) {
            index = i % HOT_REGION_SIZE; // High frequency, small locality
        }
        // 20% of accesses target the large COLD region (HOT_REGION_SIZE to TOTAL_ARRAY_SIZE-1)
        else {
            index = HOT_REGION_SIZE + (i % (TOTAL_ARRAY_SIZE - HOT_REGION_SIZE));
        }

        // Memory Access
        access_array[index] = access_array[index] + 1;
        sum += access_array[index];
    }
}

int main() {
    printf("Starting Hot Cold region access benchmark.\n");
    initialize_hot_cold();
    hot_cold_benchmark();
    printf("End of hot cold region access benchmark.\n");
    return 0;
}

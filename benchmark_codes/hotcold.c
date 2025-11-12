#include <stdio.h>

#define TOTAL_ARRAY_SIZE 4096
#define HOT_REGION_SIZE 256
#define NUM_ACCESSES 20000000

int access_array[TOTAL_ARRAY_SIZE];

void initialize_hot_cold() {
    int i;
    
    printf("Initializing hot cold region access.\n");
    
    for (i = 0; i < TOTAL_ARRAY_SIZE; i++) {
        access_array[i] = 0;
    }
}

void hot_cold_benchmark() {
    int i, index;
    int sum;

    printf("Running...\n");
    
    sum = 0;
    
    for (i = 0; i < NUM_ACCESSES; i++) {
        if ((i % 10) < 8) {
            index = i % HOT_REGION_SIZE;
        }
        else {
            index = HOT_REGION_SIZE + (i % (TOTAL_ARRAY_SIZE - HOT_REGION_SIZE));
        }

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
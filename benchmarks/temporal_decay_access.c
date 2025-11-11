#include <stdio.h>
#define PHASES 10
#define ARRAY_SIZE 10240
#define PHASE_ACCESSES 200000
#define WORKING_SET_SIZE 512

int decay_array[ARRAY_SIZE];

void initialize_decay() {
    int i;
    for (i = 0; i < ARRAY_SIZE; i++) {
        decay_array[i] = 0;
    }
}

void temporal_decay_benchmark() {
    int phase, i, index;
    int sum = 0;

    for (phase = 0; phase < PHASES; phase++) {
        // The working set shifts by one WORKING_SET_SIZE block each phase
        int current_working_set_start = (phase * WORKING_SET_SIZE) % ARRAY_SIZE;

        for (i = 0; i < PHASE_ACCESSES; i++) {

            // Access is localized within the current working set
            index = current_working_set_start + (i % WORKING_SET_SIZE);
            index = index % ARRAY_SIZE; // Ensure wrap-around

            // Memory Access
            decay_array[index] = decay_array[index] + 1;
            sum += decay_array[index];
        }
    }
}

int main() {
    printf("Starting temporal decay benchmark.\n");
    initialize_decay();
    temporal_decay_benchmark();
    printf("End of temporal decay benchmark.\n");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 10000000
#define ACCESS_COUNT 50000000

int data_array[ARRAY_SIZE];
int access_indices[ARRAY_SIZE];

void initialize_random_permutation() {
    int i, j, temp;
    int SHUFFLE_RANGE;

    SHUFFLE_RANGE = ARRAY_SIZE / 4;

    srand(1);

    for (i = 0; i < ARRAY_SIZE; i++) {
        data_array[i] = i;
        access_indices[i] = i;
    }

    // Fisher-Yates shuffle to shuffle first SHUFFLE_RANGE elements properly
    for (i = SHUFFLE_RANGE - 1; i > 0; i--) {
        j = rand() % (i + 1);

        temp = access_indices[i];
        access_indices[i] = access_indices[j];
        access_indices[j] = temp;
    }
}

void random_permutation_benchmark() {
    int i, index;
    int sum = 0;

    // Use a local pointer to avoid array lookup overhead in loop
    int *data = data_array;
    int *indices = access_indices;

    for (i = 0; i < ACCESS_COUNT; i++) {
        index = indices[i % ARRAY_SIZE];

        data[index] = data[index] + 1;
        sum += data[index];
    }

    // Use sum somewhere to prevent compiler optimization removing loop
    if (sum == 0)
        printf("Sum is zero (should not happen)\n");
}

int main() {
    printf("Starting random permutation benchmark.\n");
    initialize_random_permutation();
    random_permutation_benchmark();
    printf("End of random permutation benchmark.\n");
    return 0;
}

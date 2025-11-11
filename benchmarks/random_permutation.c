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

    for (i = SHUFFLE_RANGE - 1; i > 0; i--) {
        j = rand() % (i + 1);

        temp = access_indices[i];
        access_indices[i] = access_indices[j];
        access_indices[j] = temp;
    }
}

void random_permutation_benchmark() {
    int i, index;
    int sum;

    sum = 0;

    for (i = 0; i < ACCESS_COUNT; i++) {
        index = access_indices[i % ARRAY_SIZE];

        data_array[index] = data_array[index] + 1;
        sum += data_array[index];
    }
}

int main() {
    printf("Starting random permutation benchmark.\n");
    initialize_random_permutation();
    random_permutation_benchmark();
    printf("End of random permutatation benchmark.\n");
    return 0;
}

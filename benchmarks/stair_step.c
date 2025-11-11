#include <stdio.h>
#define MATRIX_SIZE 2048
#define BLOCK_SIZE 16
#define NUM_ITERATIONS 5

int large_matrix[MATRIX_SIZE][MATRIX_SIZE];

void initialize_matrix() {
    printf("Matrix initialization.\n");
    int i, j;
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            large_matrix[i][j] = i * MATRIX_SIZE + j;
        }
    }
}

void stair_step_access() {
    printf("Stair Step access.\n");
    int iter, block_row, block_col, i, j;
    for (iter = 0; iter < NUM_ITERATIONS; iter++) {
        for (block_row = 0; block_row < MATRIX_SIZE; block_row += BLOCK_SIZE) {
            for (block_col = 0; block_col < MATRIX_SIZE; block_col += BLOCK_SIZE) {
                for (i = block_row; i < block_row + BLOCK_SIZE && i < MATRIX_SIZE; i++) {
                    for (j = block_col; j < block_col + BLOCK_SIZE && j < MATRIX_SIZE; j++) {
                        large_matrix[i][j] = large_matrix[i][j] + 1;
                    }
                }
            }
        }
    }
}

int main() {
    printf("Starting Stair Step benchmark.");
    initialize_matrix();
    stair_step_access();
    printf("End of Stair Step benchmark.\n");
    return 0;
}

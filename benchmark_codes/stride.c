#include <stdio.h>
#include <stdlib.h>

#define STRIDE 256                    // Stride larger than cache line size (assumed 64 bytes)
#define ARRAY_SIZE (10 * 1024 * 1024) // Large array size to exceed cache

int main() {
    // Declarations moved to the top for C89 compatibility
    char *buffer;
    long i; // 'long' is now safely declared here

    buffer = (char *)malloc(ARRAY_SIZE);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Access memory with fixed stride
    for (i = 0; i < ARRAY_SIZE; i += STRIDE) {
        buffer[i] = (char)(i & 0xFF);
    }

    free(buffer);
    return 0;
}

#include <stdio.h>

#include "dynamic_math.h"

#define GOLD "analysis/time_orbit/gold_vector3_to_matrix_p3_data.bin"

static int read_gold(void *out, size_t bytes)
{
    FILE *file = fopen(GOLD, "rb");
    size_t count;
    if (file == NULL) {
        perror(GOLD);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    return fclose(file) == 0 && count == bytes ? 0 : -1;
}

static int compare(const void *actual, const void *expected, size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;
    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            fprintf(stderr, "vector3_to_matrix P3 mismatch +0x%zx: actual=%02x expected=%02x\n",
                    index, (unsigned)a[index], (unsigned)e[index]);
            return 1;
        }
    }
    fprintf(stderr, "vector3_to_matrix P3 mismatched bytes: 0/%zu\n", bytes);
    return 0;
}

int main(void)
{
    double actual[12] = {1.0, -2.0, 3.0, 99.0, 4.0, -5.0,
                         6.0, 98.0, 7.0, -8.0, 9.0, 97.0};
    DpMatrix matrix = {3, 3, 4, 0, actual};
    unsigned char expected[sizeof(actual)];

    if (read_gold(expected, sizeof(expected)) != 0) return 1;
    vector3_to_matrix(&matrix);
    if (compare(actual, expected, sizeof(actual)) != 0) return 1;
    puts("vector3_to_matrix P3 original-ELF compare: PASS (bitwise)");
    return 0;
}

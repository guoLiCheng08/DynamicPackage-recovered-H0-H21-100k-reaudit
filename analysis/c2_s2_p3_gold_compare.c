#include <stdio.h>

#include "dynamic_math.h"

static int read_gold(void *out, size_t bytes, const char *path)
{
    FILE *file = fopen(path, "rb");
    size_t count;
    if (file == NULL) {
        perror(path);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    if (count != bytes || fgetc(file) != EOF) {
        (void)fclose(file);
        return -1;
    }
    return fclose(file) == 0 ? 0 : -1;
}

static int compare(const void *actual, const void *expected, size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;
    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            fprintf(stderr, "s2/c2 P3 mismatch +0x%zx: actual=%02x expected=%02x\n",
                    index, (unsigned)a[index], (unsigned)e[index]);
            return 1;
        }
    }
    fprintf(stderr, "s2/c2 P3 mismatched bytes: 0/%zu\n", bytes);
    return 0;
}

int main(void)
{
    const double input[5] = {0.0, 1.0, -1.0, 6.283185307179586, -6.283185307179586};
    double actual[10];
    unsigned char expected[sizeof(actual)];
    size_t index;

    if (read_gold(expected, 5u * sizeof(double),
                  "analysis/time_orbit/s2_gold.bin") != 0 ||
        read_gold(expected + 5u * sizeof(double), 5u * sizeof(double),
                  "analysis/time_orbit/c2_gold.bin") != 0) return 1;
    for (index = 0u; index < 5u; ++index) actual[index] = s2(input[index]);
    for (index = 0u; index < 5u; ++index) actual[index + 5u] = c2(input[index]);
    if (compare(actual, expected, sizeof(actual)) != 0) return 1;
    puts("s2/c2 P3 original-ELF compare: PASS (bitwise)");
    return 0;
}

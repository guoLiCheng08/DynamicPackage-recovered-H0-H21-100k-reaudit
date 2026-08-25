#include <stdio.h>

#include "dynamic_math.h"

#define GOLD "analysis/time_orbit/gold_niceangle_p3_outputs.bin"

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
            fprintf(stderr, "NiceAngle P3 mismatch +0x%zx: actual=%02x expected=%02x\n",
                    index, (unsigned)a[index], (unsigned)e[index]);
            return 1;
        }
    }
    fprintf(stderr, "NiceAngle P3 mismatched bytes: 0/%zu\n", bytes);
    return 0;
}

int main(void)
{
    const double x[8] = {1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 1.0, 0.0};
    const double y[8] = {1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0};
    double actual[8];
    unsigned char expected[sizeof(actual)];
    size_t index;

    if (read_gold(expected, sizeof(expected)) != 0) return 1;
    for (index = 0u; index < 8u; ++index) actual[index] = NiceAngle(x[index], y[index]);
    if (compare(actual, expected, sizeof(actual)) != 0) return 1;
    puts("NiceAngle P3 original-ELF compare: PASS (bitwise)");
    return 0;
}

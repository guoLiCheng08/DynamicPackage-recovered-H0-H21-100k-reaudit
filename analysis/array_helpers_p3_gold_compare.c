#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

#define COPY_GOLD "analysis/time_orbit/gold_array_copy_p3_output.bin"
#define SUM_GOLD "analysis/time_orbit/gold_array_sum_p3_output.bin"

static int read_gold(const char *path, void *out, size_t bytes)
{
    FILE *file = fopen(path, "rb");
    size_t count;
    if (file == NULL) { perror(path); return -1; }
    count = fread(out, 1u, bytes, file);
    return fclose(file) == 0 && count == bytes ? 0 : -1;
}

static int compare(const char *label, const void *actual, const void *expected, size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;
    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            fprintf(stderr, "%s mismatch +0x%zx: actual=%02x expected=%02x\n", label,
                    index, (unsigned)a[index], (unsigned)e[index]);
            return 1;
        }
    }
    fprintf(stderr, "%s mismatched bytes: 0/%zu\n", label, bytes);
    return 0;
}

int main(void)
{
    double source[33];
    double copy_destination[33];
    double sum_destination[33];
    unsigned char expected_copy[sizeof(copy_destination)];
    unsigned char expected_sum[sizeof(sum_destination)];
    unsigned index;
    int mismatch;

    for (index = 0u; index < 33u; ++index) {
        source[index] = (double)index + 0.25;
        copy_destination[index] = 100.0 - (double)index;
        sum_destination[index] = 100.0 - (double)index;
    }
    ((unsigned long long *)source)[1] = 0x8000000000000000ULL;
    if (read_gold(COPY_GOLD, expected_copy, sizeof(expected_copy)) != 0 ||
        read_gold(SUM_GOLD, expected_sum, sizeof(expected_sum)) != 0) return 1;
    array_copy(copy_destination, source);
    array_sum(source, sum_destination, 0.5);
    mismatch = compare("array_copy P3", copy_destination, expected_copy,
                       sizeof(copy_destination));
    mismatch |= compare("array_sum P3", sum_destination, expected_sum,
                        sizeof(sum_destination));
    if (mismatch == 0) puts("array helpers P3 original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

#include <stdio.h>

#include "dynamic_math.h"

#define GOLD "analysis/time_orbit/gold_eccentric2true_p3_outputs.bin"

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
            fprintf(stderr, "Eccentric2True P3 mismatch +0x%zx: actual=%02x expected=%02x\n",
                    index, (unsigned)a[index], (unsigned)e[index]);
            return 1;
        }
    }
    fprintf(stderr, "Eccentric2True P3 mismatched bytes: 0/%zu\n", bytes);
    return 0;
}

int main(void)
{
    const double input[4][5] = {
        {0.1, 0.2, 1.0, 1.0, 0.5},
        {-1.0, 0.5, -1.0, 1.0, 0.2},
        {0.0, 0.0, 0.0, 1.0, 1.0},
        {3.0, 0.1, 0.0, -1.0, 0.9}
    };
    double actual[4];
    unsigned char expected[sizeof(actual)];
    size_t index;

    if (read_gold(expected, sizeof(expected)) != 0) return 1;
    for (index = 0u; index < 4u; ++index) {
        actual[index] = Eccentric2True(input[index][0], input[index][1], input[index][2],
                                       input[index][3], input[index][4]);
    }
    if (compare(actual, expected, sizeof(actual)) != 0) return 1;
    puts("Eccentric2True P3 original-ELF compare: PASS (bitwise)");
    return 0;
}

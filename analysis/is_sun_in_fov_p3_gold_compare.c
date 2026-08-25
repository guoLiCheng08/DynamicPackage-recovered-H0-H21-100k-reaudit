#include <stdio.h>

#include "dynamic_math.h"

#define GOLD "analysis/time_orbit/gold_is_sun_in_fov_p3_returns.bin"

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
            fprintf(stderr, "isSunInFOV P3 mismatch +0x%zx: actual=%02x expected=%02x\n",
                    index, (unsigned)a[index], (unsigned)e[index]);
            return 1;
        }
    }
    fprintf(stderr, "isSunInFOV P3 mismatched bytes: 0/%zu\n", bytes);
    return 0;
}

int main(void)
{
    double direction_data[4][3] = {
        {0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, -1.0},
        {0.479425538604203, 0.0, 0.8775825618903728}
    };
    const double threshold[4] = {0.5, 0.5, 3.2, 0.5};
    int actual[4];
    unsigned char expected[sizeof(actual)];
    DpVector direction = {3, 0, direction_data[0]};
    size_t index;

    if (read_gold(expected, sizeof(expected)) != 0) return 1;
    for (index = 0u; index < 4u; ++index) {
        direction.data = direction_data[index];
        actual[index] = isSunInFOV(&direction, threshold[index]);
    }
    if (compare(actual, expected, sizeof(actual)) != 0) return 1;
    puts("isSunInFOV P3 original-ELF compare: PASS (bitwise)");
    return 0;
}

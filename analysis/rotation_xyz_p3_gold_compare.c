#include <stdio.h>

#include "dynamic_math.h"

#define GOLD "analysis/time_orbit/gold_rotation_xyz_p3_data.bin"

typedef void (*RotationFn)(DpMatrix *, double);

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
            fprintf(stderr, "Rotation XYZ P3 mismatch +0x%zx: actual=%02x expected=%02x\n",
                    index, (unsigned)a[index], (unsigned)e[index]);
            return 1;
        }
    }
    fprintf(stderr, "Rotation XYZ P3 mismatched bytes: 0/%zu\n", bytes);
    return 0;
}

int main(void)
{
    const RotationFn rotation[3] = {Rotation_X, Rotation_Y, Rotation_Z};
    const double angle[3] = {0.5, -0.5, 0.0};
    double actual[81] = {0.0};
    unsigned char expected[sizeof(actual)];
    DpMatrix matrix = {3, 3, 3, 0, actual};
    size_t axis;
    size_t sample;

    if (read_gold(expected, sizeof(expected)) != 0) return 1;
    for (axis = 0u; axis < 3u; ++axis) {
        for (sample = 0u; sample < 3u; ++sample) {
            matrix.data = actual + (axis * 3u + sample) * 9u;
            rotation[axis](&matrix, angle[sample]);
        }
    }
    if (compare(actual, expected, sizeof(actual)) != 0) return 1;
    puts("Rotation XYZ P3 original-ELF compare: PASS (bitwise)");
    return 0;
}

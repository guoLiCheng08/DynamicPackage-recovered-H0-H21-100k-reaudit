#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

int main(void)
{
    double data[3];
    double scale;
    DpVector vector = {3, 0, data};
    unsigned char expected[24];
    FILE *file;

    file = fopen("analysis/math/gold_vector_scale_h0_nan.bin", "rb");
    if (file == NULL || fread(expected, 1u, sizeof(expected), file) != sizeof(expected) ||
        fclose(file) != 0) {
        return 2;
    }
    {
        const uint64_t negative_nan = UINT64_C(0xfff8000000000000);
        memcpy(&data[0], &negative_nan, sizeof(data[0]));
        memcpy(&data[1], &negative_nan, sizeof(data[1]));
        memcpy(&data[2], &negative_nan, sizeof(data[2]));
        memcpy(&scale, &negative_nan, sizeof(scale));
    }
    if (vector_scale(&vector, scale) != 0) {
        return 3;
    }
    if (memcmp(data, expected, sizeof(expected)) != 0) {
        uint64_t bits[3];
        memcpy(bits, data, sizeof(bits));
        (void)fprintf(stderr, "vector_scale NaN mismatch: %016llx %016llx %016llx\n",
                      (unsigned long long)bits[0], (unsigned long long)bits[1],
                      (unsigned long long)bits[2]);
        return 1;
    }
    puts("vector_scale H0 negative-NaN original-ELF compare: PASS (bitwise)");
    return 0;
}

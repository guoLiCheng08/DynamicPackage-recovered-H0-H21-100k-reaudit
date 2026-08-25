#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

#define GOLD_PATH "analysis/time_orbit/gold_vector_set_zero_nonfatal_p3_snapshots.bin"

int main(void)
{
    double expected[6];
    double actual[6];
    double data[3] = {1.5, -2.0, 0.0};
    DpVector vector = {0, 0, data};
    FILE *file;
    size_t index;
    unsigned mismatches = 0u;

    file = fopen(GOLD_PATH, "rb");
    if (file == NULL) { perror(GOLD_PATH); return 1; }
    if (fread(expected, 1u, sizeof(expected), file) != sizeof(expected) || fclose(file) != 0) return 1;
    vector_set_zero(&vector);
    memcpy(actual, data, sizeof(data));
    vector.count = 3;
    vector_set_zero(&vector);
    memcpy(actual + 3, data, sizeof(data));
    for (index = 0u; index < sizeof(actual); ++index) {
        if (((const unsigned char *)actual)[index] != ((const unsigned char *)expected)[index]) ++mismatches;
    }
    printf("vector_set_zero P3 snapshots mismatched bytes: %u/%zu\n", mismatches, sizeof(actual));
    if (mismatches == 0u) puts("vector_set_zero nonfatal P3 original-ELF compare: PASS (bitwise)");
    return mismatches == 0u ? 0 : 1;
}

#include <stdio.h>

#include "dynamic_math.h"

#define GOLD_PATH "analysis/time_orbit/gold_vector_square_nonfatal_p3_results.bin"

int main(void)
{
    double expected[2];
    double actual[2];
    double data[3] = {3.0, -4.0, 12.0};
    DpVector vector = {0, 0, data};
    FILE *file;
    size_t index;
    unsigned mismatches = 0u;

    file = fopen(GOLD_PATH, "rb");
    if (file == NULL) { perror(GOLD_PATH); return 1; }
    if (fread(expected, 1u, sizeof(expected), file) != sizeof(expected) || fclose(file) != 0) return 1;
    actual[0] = vector_square(&vector);
    vector.count = 3;
    actual[1] = vector_square(&vector);
    for (index = 0u; index < sizeof(actual); ++index) {
        if (((const unsigned char *)actual)[index] != ((const unsigned char *)expected)[index]) ++mismatches;
    }
    printf("vector_square P3 results mismatched bytes: %u/%zu\n", mismatches, sizeof(actual));
    if (mismatches == 0u) puts("vector_square nonfatal P3 original-ELF compare: PASS (bitwise)");
    return mismatches == 0u ? 0 : 1;
}

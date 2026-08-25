#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

#define GOLD_PATH "analysis/time_orbit/gold_matrix_set_all_nonfatal_p3_snapshots.bin"

int main(void)
{
    double expected[24];
    double actual[24];
    double data[12];
    DpMatrix matrix = {0, 2, 4, 0, data};
    FILE *file;
    unsigned index;
    unsigned mismatches = 0u;

    file = fopen(GOLD_PATH, "rb");
    if (file == NULL) { perror(GOLD_PATH); return 1; }
    if (fread(expected, 1u, sizeof(expected), file) != sizeof(expected) || fclose(file) != 0) return 1;
    for (index = 0u; index < 12u; ++index) data[index] = (double)index + 1.0;
    matrix_set_all(&matrix, -1.25);
    memcpy(actual, data, sizeof(data));
    matrix.rows = 2;
    matrix_set_all(&matrix, -1.25);
    memcpy(actual + 12, data, sizeof(data));
    for (index = 0u; index < sizeof(actual); ++index) {
        if (((const unsigned char *)actual)[index] != ((const unsigned char *)expected)[index]) ++mismatches;
    }
    printf("matrix_set_all P3 snapshots mismatched bytes: %u/%zu\n", mismatches, sizeof(actual));
    if (mismatches == 0u) puts("matrix_set_all nonfatal P3 original-ELF compare: PASS (bitwise)");
    return mismatches == 0u ? 0 : 1;
}

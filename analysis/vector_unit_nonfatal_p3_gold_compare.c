#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

#define GOLD_PATH "analysis/time_orbit/gold_vector_unit_nonfatal_p3_destination.bin"

int main(void)
{
    double expected[3];
    double source_data[3] = {0.0, 0.0, 0.0};
    double destination_data[3] = {9.0, 8.0, 7.0};
    DpVector source = {3, 0, source_data};
    DpVector destination = {1, 0, destination_data};
    FILE *file;
    size_t index;
    unsigned mismatches = 0u;

    file = fopen(GOLD_PATH, "rb");
    if (file == NULL) { perror(GOLD_PATH); return 1; }
    if (fread(expected, 1u, sizeof(expected), file) != sizeof(expected) || fclose(file) != 0) return 1;
    vector_unit(&destination, &source);
    source_data[0] = 3.0;
    source_data[1] = 4.0;
    source_data[2] = 12.0;
    vector_unit(&destination, &source);
    for (index = 0u; index < sizeof(destination_data); ++index) {
        if (((const unsigned char *)destination_data)[index] != ((const unsigned char *)expected)[index]) ++mismatches;
    }
    printf("vector_unit P3 destination mismatched bytes: %u/%zu\n", mismatches,
           sizeof(destination_data));
    if (mismatches == 0u) puts("vector_unit nonfatal P3 original-ELF compare: PASS (bitwise)");
    return mismatches == 0u ? 0 : 1;
}

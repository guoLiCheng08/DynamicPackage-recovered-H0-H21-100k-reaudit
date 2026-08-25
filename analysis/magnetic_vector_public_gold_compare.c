#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "dynamic_environment.h"
#define GOLD_PATH "analysis/time_orbit/gold_magnetic_vector_step2.bin"
int main(void)
{
    double gold[7];
    double actual[3] = {0.0, 0.0, 0.0};
    FILE *file = fopen(GOLD_PATH, "rb");
    unsigned int i;
    if (file == NULL || fread(gold, sizeof(double), 7u, file) != 7u || fclose(file) != 0) return 2;
    MagneticVector(actual, gold[0], gold[1], gold[2], gold[3]);
    for (i = 0u; i < 3u; ++i) {
        uint64_t observed, expected;
        memcpy(&observed, &actual[i], sizeof(observed));
        memcpy(&expected, &gold[i + 4u], sizeof(expected));
        if (observed != expected) return 1;
    }
    puts("MagneticVector public ABI original-ELF gold compare: PASS (bitwise)");
    return 0;
}

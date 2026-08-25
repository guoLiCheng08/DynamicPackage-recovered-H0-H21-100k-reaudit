#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_environment.h"

#define GOLD_PATH "analysis/time_orbit/gold_magnetic_vector_step2.bin"

static int read_gold(double gold[7])
{
    FILE *file = fopen(GOLD_PATH, "rb");
    if (file == NULL) {
        perror(GOLD_PATH);
        return -1;
    }
    if (fread(gold, sizeof(gold[0]), 7u, file) != 7u || fclose(file) != 0) {
        return -1;
    }
    return 0;
}

int main(void)
{
    double gold[7];
    double actual[3] = {0.0};
    unsigned index;

    if (read_gold(gold) != 0) return 1;
    dp_magnetic_vector(actual, gold[0], gold[1], gold[2], gold[3]);
    for (index = 0u; index < 3u; ++index) {
        uint64_t actual_bits;
        uint64_t expected_bits;
        memcpy(&actual_bits, &actual[index], sizeof(actual_bits));
        memcpy(&expected_bits, &gold[index + 4u], sizeof(expected_bits));
        if (actual_bits != expected_bits) {
            fprintf(stderr,
                    "MagneticVector[%u] got=%016llx expected=%016llx\n",
                    index, (unsigned long long)actual_bits,
                    (unsigned long long)expected_bits);
            return 1;
        }
    }
    puts("MagneticVector original-ELF gold compare: PASS (bitwise)");
    return 0;
}

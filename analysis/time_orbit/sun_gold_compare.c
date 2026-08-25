#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_environment.h"

#define GOLD_PATH "analysis/time_orbit/gold_sun_step2.bin"

static int read_gold(double gold[4])
{
    FILE *file = fopen(GOLD_PATH, "rb");
    if (file == NULL) {
        perror(GOLD_PATH);
        return -1;
    }
    if (fread(gold, sizeof(gold[0]), 4u, file) != 4u || fclose(file) != 0) {
        return -1;
    }
    return 0;
}

int main(void)
{
    double gold[4];
    double actual_data[3] = {0.0};
    DpVector actual = {3, 0, actual_data};
    unsigned index;

    if (read_gold(gold) != 0) return 1;
    dp_sun_vector(gold[0], &actual);
    for (index = 0u; index < 3u; ++index) {
        uint64_t expected_bits;
        uint64_t actual_bits;
        memcpy(&expected_bits, &gold[index + 1u], sizeof(expected_bits));
        memcpy(&actual_bits, &actual_data[index], sizeof(actual_bits));
        if (actual_bits != expected_bits) {
            fprintf(stderr,
                    "Sun[%u] got=%016llx expected=%016llx\n",
                    index, (unsigned long long)actual_bits,
                    (unsigned long long)expected_bits);
            return 1;
        }
    }
    puts("Sun original-ELF gold compare: PASS (bitwise)");
    return 0;
}

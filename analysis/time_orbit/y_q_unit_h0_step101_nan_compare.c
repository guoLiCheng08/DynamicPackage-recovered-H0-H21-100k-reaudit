#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

static uint64_t bits(double value)
{
    uint64_t result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    uint64_t neg_nan_bits = UINT64_C(0xfff8000000000000);
    double state[33] = {0.0};
    double expected[4];
    FILE *file;
    unsigned index;

    for (index = 0u; index < 4u; ++index) {
        memcpy(&state[index], &neg_nan_bits, sizeof(state[index]));
    }
    file = fopen("analysis/time_orbit/gold_y_q_unit_h0_step101_neg_nan.bin", "rb");
    if (file == NULL || fread(expected, 1u, sizeof(expected), file) != sizeof(expected) ||
        fclose(file) != 0) {
        return 2;
    }
    y_q_unit(state);
    for (index = 0u; index < 4u; ++index) {
        if (bits(state[index]) != bits(expected[index])) {
            printf("y_q_unit neg-NaN[%u] actual=%016" PRIx64 " expected=%016" PRIx64 "\n",
                   index, bits(state[index]), bits(expected[index]));
            return 1;
        }
    }
    puts("y_q_unit H0 step101 neg-NaN original-ELF compare: PASS (bitwise)");
    return 0;
}

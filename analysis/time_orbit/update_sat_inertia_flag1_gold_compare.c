#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_flex.h"

static double from_bits(uint64_t value)
{
    double result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static int compare_matrix(const char *label, const double actual[9],
                          const uint64_t expected[9])
{
    unsigned index;
    for (index = 0u; index < 9u; ++index) {
        if (bits(actual[index]) != expected[index]) {
            printf("FAIL %s[%u] actual=%a expected_bits=%016" PRIx64
                   " actual_bits=%016" PRIx64 "\n", label, index,
                   actual[index], expected[index], bits(actual[index]));
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    const uint64_t expected_inertia[9] = {
        UINT64_C(0x405e000000000000), UINT64_C(0xbfe428f5c28f5c29), UINT64_C(0x3ff4cccccccccccd),
        UINT64_C(0xbfe428f5c28f5c29), UINT64_C(0x4076900000000000), UINT64_C(0xbfe0000000000000),
        UINT64_C(0x3ff4cccccccccccd), UINT64_C(0xbfe0000000000000), UINT64_C(0x407d500000000000)
    };
    const uint64_t expected_inverse[9] = {
        UINT64_C(0x3f81113cdb6f56f3), UINT64_C(0x3eee6ed1eab39b43), UINT64_C(0xbef8349fe3dcda19),
        UINT64_C(0x3eee6ed1eab39b43), UINT64_C(0x3f66b158cdc4be55), UINT64_C(0x3ec86f97ee84395f),
        UINT64_C(0xbef8349fe3dcda19), UINT64_C(0x3ec86f97ee84395f), UINT64_C(0x3f6177ae1e1da471)
    };
    double baseline[9];
    double inertia[9] = {0.0};
    double inverse[9] = {0.0};
    unsigned index;

    for (index = 0u; index < 9u; ++index) baseline[index] = from_bits(expected_inertia[index]);
    dp_reset_inertia_from_baseline_3x3(inertia, inverse, baseline);
    if (compare_matrix("inertia", inertia, expected_inertia) != 0 ||
        compare_matrix("inverse", inverse, expected_inverse) != 0) return 1;
    puts("Update_sat_inertia_xw flag=1 original-ELF gold compare: PASS (bitwise)");
    return 0;
}

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_flex.h"
#include "dynamic_sat_inertia_flag0_constants.inc"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static int compare_block(const char *label, const double *actual,
                         const uint64_t *expected, size_t count)
{
    size_t index;
    for (index = 0u; index < count; ++index) {
        if (bits(actual[index]) != expected[index]) {
            printf("FAIL %s[%zu] actual=%a expected_bits=%016" PRIx64
                   " actual_bits=%016" PRIx64 "\n", label, index,
                   actual[index], expected[index], bits(actual[index]));
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    double inertia[9] = {0.0};
    double inverse[9] = {0.0};
    double m3_e8[9] = {0.0};
    double m3_448[9] = {0.0};
    double m3_4a8[9] = {0.0};
    double m3_508[9] = {0.0};
    double m3_568[9] = {0.0};
    double m6[30] = {0.0};
    double coupling[30] = {0.0};
    double m9[9] = {0.0};
    double ma[100] = {0.0};
    double md[100] = {0.0};
    DpSatInertiaFlag0Outputs output = {
        inertia, inverse, m3_e8, m3_448, m3_4a8, m3_508, m3_568,
        m6, coupling, m9, ma, md
    };

    if (dp_load_sat_inertia_flag0_static(&output) != 0 ||
        compare_block("inertia", inertia, dp_flag0_inertia_bits, 9u) != 0 ||
        compare_block("inverse", inverse, dp_flag0_inverse_bits, 9u) != 0 ||
        compare_block("m3_e8", m3_e8, dp_flag0_m3_e8_bits, 9u) != 0 ||
        compare_block("m3_448", m3_448, dp_flag0_m3_448_bits, 9u) != 0 ||
        compare_block("m3_4a8", m3_4a8, dp_flag0_m3_4a8_bits, 9u) != 0 ||
        compare_block("m3_508", m3_508, dp_flag0_m3_508_bits, 9u) != 0 ||
        compare_block("m3_568", m3_568, dp_flag0_m3_568_bits, 9u) != 0 ||
        compare_block("m6", m6, dp_flag0_m6_bits, 30u) != 0 ||
        compare_block("coupling", coupling, dp_flag0_c_bits, 30u) != 0 ||
        compare_block("m9", m9, dp_flag0_m9_bits, 9u) != 0 ||
        compare_block("ma", ma, dp_flag0_ma_bits, 100u) != 0 ||
        compare_block("md", md, dp_flag0_md_bits, 100u) != 0) return 1;

    puts("Update_sat_inertia_xw flag=0 default original-ELF gold compare: PASS (bitwise)");
    return 0;
}

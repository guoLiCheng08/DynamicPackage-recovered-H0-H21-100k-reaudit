#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_dynamics.h"

static const uint64_t expected_out_bits[3] = {
    UINT64_C(0xbfcb851eb851eb84), UINT64_C(0x3f947ae147ae1485),
    UINT64_C(0xbfa1eb851eb851ec)
};
static const uint64_t expected_term_bits[3] = {
    UINT64_C(0x3fb999999999999a), UINT64_C(0x3fc999999999999a),
    UINT64_C(0xbfd3333333333333)
};

static int compare_bits(const char *label, const double actual[3],
                        const uint64_t expected[3])
{
    size_t index;

    for (index = 0u; index < 3u; ++index) {
        uint64_t bits;

        memcpy(&bits, &actual[index], sizeof(bits));
        if (bits != expected[index]) {
            (void)printf("%s[%zu] mismatch: got=%#llx expected=%#llx\n", label, index,
                         (unsigned long long)bits, (unsigned long long)expected[index]);
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    double output_data[3] = {0.0, 0.0, 0.0};
    double term_data[3] = {0.1, 0.2, -0.3};
    double rate_data[3] = {0.2, -0.3, 0.4};
    double additional_data[3] = {1.0, -2.0, 0.5};
    double inertia_data[9] = {2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 4.0};
    double inverse_data[9] = {0.5, 0.0, 0.0, 0.0, 1.0 / 3.0, 0.0, 0.0, 0.0, 0.25};
    DpVector output = {3, 0, output_data};
    DpVector term = {3, 0, term_data};
    DpVector rate = {3, 0, rate_data};
    DpVector additional = {3, 0, additional_data};
    DpMatrix inertia = {3, 3, 3, 0, inertia_data};
    DpMatrix inverse = {3, 3, 3, 0, inverse_data};

    dynamics(&output, &term, &rate, &additional, &inertia, &inverse);
    if (compare_bits("dynamics output", output_data, expected_out_bits) != 0 ||
        compare_bits("dynamics term", term_data, expected_term_bits) != 0) return 1;
    (void)puts("PASS: dynamics controlled original-ELF gold compare");
    return 0;
}

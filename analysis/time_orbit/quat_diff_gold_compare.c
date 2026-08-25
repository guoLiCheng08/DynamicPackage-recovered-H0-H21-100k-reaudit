#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    double q_xyz_data[3] = {0.1, -0.2, 0.3};
    double out_xyz_data[3] = {0.0, 0.0, 0.0};
    double rate_data[3] = {0.01, -0.02, 0.03};
    DpQuatAbi q = {0.8, {3, 0, q_xyz_data}};
    DpQuatAbi out = {0.0, {3, 0, out_xyz_data}};
    DpVector rate = {3, 0, rate_data};
    const uint64_t expected[4] = {
        UINT64_C(0xbf7cac083126e978), UINT64_C(0x3f70624dd2f1a9fc),
        UINT64_C(0xbf80624dd2f1a9fc), UINT64_C(0x3f889374bc6a7efa)
    };
    const double actual[4] = {out.w, out_xyz_data[0], out_xyz_data[1], out_xyz_data[2]};
    unsigned index;

    quat_diff(&out, &q, &rate);
    for (index = 0u; index < 4u; ++index) {
        const double value = index == 0u ? out.w : out_xyz_data[index - 1u];
        if (bits(value) != expected[index]) {
            printf("FAIL field=%u actual=%a expected_bits=%016llx actual_bits=%016llx\n",
                   index, value, (unsigned long long)expected[index],
                   (unsigned long long)bits(value));
            return 1;
        }
    }
    (void)actual;
    puts("quat_diff original-ELF gold compare: PASS (bitwise)");
    return 0;
}

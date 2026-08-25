#include <inttypes.h>
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
    uint64_t neg_nan_bits = UINT64_C(0xfff8000000000000);
    double q_xyz[3];
    double omega_data[3];
    double out_xyz[3] = {0.0};
    double expected[4];
    DpQuatAbi quat;
    DpQuatAbi output;
    DpVector omega = {3, 0, omega_data};
    FILE *file;
    unsigned index;

    for (index = 0u; index < 3u; ++index) {
        memcpy(&q_xyz[index], &neg_nan_bits, sizeof(q_xyz[index]));
        memcpy(&omega_data[index], &neg_nan_bits, sizeof(omega_data[index]));
    }
    memcpy(&quat.w, &neg_nan_bits, sizeof(quat.w));
    quat.xyz.count = 3;
    quat.xyz.reserved_04 = 0;
    quat.xyz.data = q_xyz;
    output.w = 0.0;
    output.xyz.count = 3;
    output.xyz.reserved_04 = 0;
    output.xyz.data = out_xyz;

    file = fopen("analysis/time_orbit/gold_quat_diff_h0_step101_neg_nan.bin", "rb");
    if (file == NULL || fread(expected, 1u, sizeof(expected), file) != sizeof(expected) ||
        fclose(file) != 0) {
        return 2;
    }
    quat_diff(&output, &quat, &omega);
    printf("quat_diff actual=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 "\\n",
           bits(output.w), bits(output.xyz.data[0]), bits(output.xyz.data[1]),
           bits(output.xyz.data[2]));
    if (bits(output.w) != bits(expected[0])) {
        printf("quat_diff w actual=%016" PRIx64 " expected=%016" PRIx64 "\n",
               bits(output.w), bits(expected[0]));
        return 1;
    }
    for (index = 0u; index < 3u; ++index) {
        if (bits(output.xyz.data[index]) != bits(expected[index + 1u])) {
            printf("quat_diff xyz[%u] actual=%016" PRIx64 " expected=%016" PRIx64 "\n",
                   index, bits(output.xyz.data[index]), bits(expected[index + 1u]));
            return 1;
        }
    }
    puts("quat_diff H0 step101 neg-NaN original-ELF compare: PASS (bitwise)");
    return 0;
}

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static int compare_vector(const char *label, const double *actual,
                          const uint64_t *expected, unsigned count)
{
    unsigned index;
    for (index = 0u; index < count; ++index) {
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
    const uint64_t expected_h[3] = {
        UINT64_C(0x3f70624dd2f1a9fd), UINT64_C(0xbf80624dd2f1a9fd),
        UINT64_C(0x3f847ae147ae147c)
    };
    const uint64_t expected_torque[3] = {
        UINT64_C(0x3f947ae147ae147c), UINT64_C(0xbfa47ae147ae147c),
        UINT64_C(0x3fa999999999999a)
    };
    const uint64_t expected_omega[4] = {
        UINT64_C(0x3fd5c28f5c28f5c2), UINT64_C(0xbfdeb851eb851eb9),
        UINT64_C(0x3fe3333333333333), UINT64_C(0xbfe6666666666666)
    };
    const uint64_t expected_momentum[4] = {
        UINT64_C(0x3fa16872b020c49b), UINT64_C(0xbfa89374bc6a7efb),
        UINT64_C(0x3faeb851eb851eb8), UINT64_C(0xbfb1eb851eb851eb)
    };
    DpReactionWheelRecovered wheels[DP_WHEEL_COUNT];
    double h_data[3] = {0.0, 0.0, 0.0};
    double torque_data[3] = {0.0, 0.0, 0.0};
    double mapping_data[12] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0
    };
    DpVector h = {3, 0, h_data};
    DpVector torque = {3, 0, torque_data};
    DpMatrix mapping = {3, 4, 4, 0, mapping_data};
    const double omega[4] = {0.3, -0.4, 0.5, -0.6};
    const double acceleration[4] = {0.2, -0.4, 0.5, -0.5};
    unsigned index;

    memset(wheels, 0, sizeof(wheels));
    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        wheels[index].inertia = 0.1;
        wheels[index].omega = omega[index];
        wheels[index].acceleration = acceleration[index];
    }
    if (dp_update_wheel_group(wheels, 0.2, &h, &torque, &mapping) != 0 ||
        compare_vector("group_h", h_data, expected_h, 3u) != 0 ||
        compare_vector("group_torque", torque_data, expected_torque, 3u) != 0) return 1;
    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        if (bits(wheels[index].omega) != expected_omega[index] ||
            bits(wheels[index].angular_momentum) != expected_momentum[index]) {
            printf("FAIL wheel[%u] omega_bits=%016" PRIx64 " momentum_bits=%016" PRIx64 "\n",
                   index, bits(wheels[index].omega), bits(wheels[index].angular_momentum));
            return 1;
        }
    }
    puts("UpdateWheel group original-ELF gold compare: PASS (bitwise)");
    return 0;
}

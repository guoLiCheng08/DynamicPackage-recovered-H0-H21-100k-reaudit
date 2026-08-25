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

static int compare_wheel_value(const char *label, unsigned wheel, double actual,
                               uint64_t expected)
{
    if (bits(actual) != expected) {
        printf("FAIL %s wheel=%u actual=%a expected_bits=%016" PRIx64
               " actual_bits=%016" PRIx64 "\n",
               label, wheel, actual, expected, bits(actual));
        return 1;
    }
    return 0;
}

static void initialize_wheels(DpReactionWheelRecovered wheels[DP_WHEEL_COUNT])
{
    unsigned index;
    memset(wheels, 0, DP_WHEEL_COUNT * sizeof(*wheels));
    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        wheels[index].add_gaussian_noise_flag = 0u;
        wheels[index].omega_limit = 1.0;
        wheels[index].torque_limit = 0.05;
        wheels[index].inertia = 0.1;
    }
}

int main(void)
{
    const uint64_t expected_case1[DP_WHEEL_COUNT] = {
        UINT64_C(0x3fc9999999999999), UINT64_C(0xbfd9999999999999),
        UINT64_C(0x0000000000000000), UINT64_C(0xbfe0000000000000)
    };
    const uint64_t expected_case2[DP_WHEEL_COUNT] = {
        UINT64_C(0x3fc9999999999999), UINT64_C(0xbfd9999999999999),
        UINT64_C(0x0000000000000000), UINT64_C(0x3fc9999999999999)
    };
    const uint64_t expected_update[DP_WHEEL_COUNT][3] = {
        {UINT64_C(0x3fd5c28f5c28f5c2), UINT64_C(0x3fc999999999999a), UINT64_C(0x3fa16872b020c49b)},
        {UINT64_C(0xbfdeb851eb851eb9), UINT64_C(0xbfd999999999999a), UINT64_C(0xbfa89374bc6a7efb)},
        {UINT64_C(0x3fe3333333333333), UINT64_C(0x3fe0000000000000), UINT64_C(0x3faeb851eb851eb8)},
        {UINT64_C(0xbfe6666666666666), UINT64_C(0xbfe0000000000000), UINT64_C(0xbfb1eb851eb851eb)}
    };
    DpReactionWheelRecovered wheels[DP_WHEEL_COUNT];
    double commands[DP_WHEEL_COUNT] = {0.02, -0.04, 0.10, -0.10};
    unsigned index;

    initialize_wheels(wheels);
    wheels[2].omega = 1.0;
    wheels[3].omega = -1.0;
    dp_set_wheel_acc(wheels, commands, NULL, NULL);
    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        if (compare_wheel_value("set_acc_case1", index, wheels[index].acceleration,
                                expected_case1[index]) != 0) return 1;
    }

    commands[2] = -0.02;
    commands[3] = 0.02;
    dp_set_wheel_acc(wheels, commands, NULL, NULL);
    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        if (compare_wheel_value("set_acc_case2", index, wheels[index].acceleration,
                                expected_case2[index]) != 0) return 1;
    }

    wheels[0].omega = 0.3; wheels[0].acceleration = 0.2;
    wheels[1].omega = -0.4; wheels[1].acceleration = -0.4;
    wheels[2].omega = 0.5; wheels[2].acceleration = 0.5;
    wheels[3].omega = -0.6; wheels[3].acceleration = -0.5;
    dp_update_wheel(wheels, 0.2);
    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        if (compare_wheel_value("update_omega", index, wheels[index].omega,
                                expected_update[index][0]) != 0 ||
            compare_wheel_value("update_acceleration", index, wheels[index].acceleration,
                                expected_update[index][1]) != 0 ||
            compare_wheel_value("update_momentum", index, wheels[index].angular_momentum,
                                expected_update[index][2]) != 0) return 1;
    }

    puts("wheel actuator original-ELF gold compare: PASS (bitwise)");
    return 0;
}

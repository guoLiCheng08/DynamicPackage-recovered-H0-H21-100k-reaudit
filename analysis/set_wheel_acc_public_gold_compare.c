#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_devices.h"

static int read_acceleration_gold(const char *path, double expected[DP_WHEEL_COUNT])
{
    FILE *file = fopen(path, "rb");
    return file != NULL && fread(expected, sizeof(*expected), DP_WHEEL_COUNT, file) == DP_WHEEL_COUNT && fclose(file) == 0;
}

static int compare_acceleration(const double expected[DP_WHEEL_COUNT])
{
    unsigned index;
    for (index = 0; index < DP_WHEEL_COUNT; ++index) {
        if (memcmp(&RWheel[index].acceleration, &expected[index], sizeof(expected[index])) != 0) return 0;
    }
    return 1;
}

int main(void)
{
    static const double bounded_command[DP_WHEEL_COUNT] = {12.0, 4.0, 4.0, -4.0};
    static const double noise_command[DP_WHEEL_COUNT] = {0.0, 0.0, 4.0, 0.0};
    double expected[DP_WHEEL_COUNT];
    unsigned index;

    if (!read_acceleration_gold("analysis/coverage_inventory/gold_set_wheel_acc_p1_accel.bin", expected)) return 2;
    dp_device_globals_reset();
    for (index = 0; index < DP_WHEEL_COUNT; ++index) {
        RWheel[index].add_gaussian_noise_flag = 0u;
        RWheel[index].omega_limit = 100.0;
        RWheel[index].torque_limit = 10.0;
        RWheel[index].inertia = 2.0;
        RWheel[index].acceleration = -99.0;
    }
    RWheel[0].omega = 0.0;
    RWheel[1].omega = 100.0;
    RWheel[2].omega = 120.0;
    RWheel[3].omega = -120.0;
    SetWheelAcc(bounded_command);
    if (!compare_acceleration(expected)) return 1;

    if (!read_acceleration_gold("analysis/coverage_inventory/gold_set_wheel_acc_noise_seeded_p1_accel.bin", expected)) return 2;
    dp_device_globals_reset();
    for (index = 0; index < DP_WHEEL_COUNT; ++index) {
        RWheel[index].add_gaussian_noise_flag = 0u;
        RWheel[index].omega_limit = 100.0;
        RWheel[index].torque_limit = 10.0;
        RWheel[index].inertia = 2.0;
        RWheel[index].omega = 0.0;
        RWheel[index].acceleration = -99.0;
    }
    RWheel[2].add_gaussian_noise_flag = 1u;
    RWheel[2].acceleration_noise_sigma = 0.5;
    srand(1);
    SetWheelAcc(noise_command);
    if (!compare_acceleration(expected)) return 1;

    puts("SetWheelAcc public ABI original-ELF compare: PASS (bitwise, limits + overspeed + seeded Gaussian noise)");
    return 0;
}

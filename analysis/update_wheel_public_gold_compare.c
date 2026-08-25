#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"

static int read_doubles(const char *path, double *out, unsigned count)
{
    FILE *file = fopen(path, "rb");
    return file != NULL && fread(out, sizeof(*out), count, file) == count && fclose(file) == 0;
}

int main(void)
{
    static const double mapping[12] = {
        1.0, -2.0, 0.5, 3.0,
        -0.25, 1.5, 2.0, -3.0,
        4.0, -1.5, 0.25, 0.5
    };
    static const double inertia[4] = {2.0, 3.0, 4.0, 5.0};
    static const double omega[4] = {10.0, -20.0, 30.0, -40.0};
    static const double acceleration[4] = {1.0, -2.0, 0.5, -1.5};
    static const double h_initial[3] = {100.0, 200.0, 300.0};
    double expected_derived[12];
    double actual_derived[12];
    double expected_torque[3];
    double expected_h[3];
    unsigned index;

    if (!read_doubles("analysis/coverage_inventory/gold_update_wheel_p1_derived.bin", expected_derived, 12u) ||
        !read_doubles("analysis/coverage_inventory/gold_update_wheel_p1_group_torque.bin", expected_torque, 3u) ||
        !read_doubles("analysis/coverage_inventory/gold_update_wheel_p1_group_h.bin", expected_h, 3u)) return 2;
    dp_device_globals_reset();
    for (index = 0; index < DP_WHEEL_COUNT; ++index) {
        RWheel[index].inertia = inertia[index];
        RWheel[index].omega = omega[index];
        RWheel[index].acceleration = acceleration[index];
    }
    memcpy(WheelGroup.angular_momentum.data, h_initial, sizeof(h_initial));
    memcpy(WheelGroup.mapping_3x4.data, mapping, sizeof(mapping));
    UpdateWheel(0.25);
    for (index = 0; index < DP_WHEEL_COUNT; ++index) {
        actual_derived[index * 3u + 0u] = RWheel[index].torque;
        actual_derived[index * 3u + 1u] = RWheel[index].omega;
        actual_derived[index * 3u + 2u] = RWheel[index].angular_momentum;
    }
    if (memcmp(actual_derived, expected_derived, sizeof(expected_derived)) != 0 ||
        memcmp(WheelGroup.torque.data, expected_torque, sizeof(expected_torque)) != 0 ||
        memcmp(WheelGroup.angular_momentum.data, expected_h, sizeof(expected_h)) != 0) return 1;
    puts("UpdateWheel public ABI original-ELF compare: PASS (bitwise, 4-wheel integration + 3x4 map + group H)");
    return 0;
}

#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"

static int read_three(const char *path, double out[3])
{
    FILE *file = fopen(path, "rb");
    return file != NULL && fread(out, sizeof(*out), 3u, file) == 3u && fclose(file) == 0;
}

static int compare_outputs(const double expected_force[3], const double expected_torque[3])
{
    return memcmp(Thruster.force_output.data, expected_force, 3u * sizeof(double)) == 0 &&
           memcmp(Thruster.torque_output.data, expected_torque, 3u * sizeof(double)) == 0;
}

int main(void)
{
    double expected_force[3];
    double expected_torque[3];
    static const double lever[3] = {1.0, -2.0, 3.0};
    static const double input[3] = {4.0, -5.0, 6.0};
    static const double off_force_seed[3] = {-10.0, -20.0, -30.0};
    static const double off_torque_seed[3] = {11.0, 22.0, 33.0};

    if (!read_three("analysis/coverage_inventory/gold_update_thruster_p1_on_force.bin", expected_force) ||
        !read_three("analysis/coverage_inventory/gold_update_thruster_p1_on_torque.bin", expected_torque)) return 2;
    dp_device_globals_reset();
    Thruster.force_scale = 2.5;
    memcpy(Thruster.lever_arm.data, lever, sizeof(lever));
    memcpy(Thruster.force_input.data, input, sizeof(input));
    Thruster.work_status = 1u;
    UpdateThruster();
    if (!compare_outputs(expected_force, expected_torque)) return 1;

    if (!read_three("analysis/coverage_inventory/gold_update_thruster_p1_off_force.bin", expected_force) ||
        !read_three("analysis/coverage_inventory/gold_update_thruster_p1_off_torque.bin", expected_torque)) return 2;
    memcpy(Thruster.force_output.data, off_force_seed, sizeof(off_force_seed));
    memcpy(Thruster.torque_output.data, off_torque_seed, sizeof(off_torque_seed));
    Thruster.work_status = 2u;
    UpdateThruster();
    if (!compare_outputs(expected_force, expected_torque)) return 1;
    puts("UpdateThruster public ABI original-ELF compare: PASS (bitwise, active force/cross + inactive zero)");
    return 0;
}

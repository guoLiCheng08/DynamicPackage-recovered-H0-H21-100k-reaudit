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

static int compare_vector(const char *label, const double actual[3],
                          const uint64_t expected[3])
{
    unsigned index;
    for (index = 0u; index < 3u; ++index) {
        if (bits(actual[index]) != expected[index]) {
            printf("FAIL %s[%u] actual=%a expected_bits=%016" PRIx64
                   " actual_bits=%016" PRIx64 "\n",
                   label, index, actual[index], expected[index], bits(actual[index]));
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    const uint64_t zero[3] = {
        UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000),
        UINT64_C(0x0000000000000000)
    };
    const uint64_t expected_force[3] = {
        UINT64_C(0x4036800000000000), UINT64_C(0x4034000000000000),
        UINT64_C(0x4031800000000000)
    };
    const uint64_t expected_torque[3] = {
        UINT64_C(0xc039000000000000), UINT64_C(0x4049000000000000),
        UINT64_C(0xc039000000000000)
    };
    double lever_arm_data[3] = {1.0, 2.0, 3.0};
    double force_input_data[3] = {9.0, 8.0, 7.0};
    double force_output_data[3] = {-0.5, 4.0, 1.5};
    double torque_output_data[3] = {6.0, 5.0, 4.0};
    DpThrusterRecovered thruster;

    memset(&thruster, 0, sizeof(thruster));
    thruster.force_scale = 2.5;
    thruster.lever_arm.count = 3;
    thruster.lever_arm.data = lever_arm_data;
    thruster.force_input.count = 3;
    thruster.force_input.data = force_input_data;
    thruster.force_output.count = 3;
    thruster.force_output.data = force_output_data;
    thruster.torque_output.count = 3;
    thruster.torque_output.data = torque_output_data;

    dp_set_thruster_work_status(&thruster, 0u);
    if (dp_update_thruster(&thruster) != 0 ||
        compare_vector("disabled_force", force_output_data, zero) != 0 ||
        compare_vector("disabled_torque", torque_output_data, zero) != 0) return 1;

    dp_set_thruster_work_status(&thruster, 1u);
    if (dp_update_thruster(&thruster) != 0 ||
        compare_vector("enabled_force", force_output_data, expected_force) != 0 ||
        compare_vector("enabled_torque", torque_output_data, expected_torque) != 0) return 1;

    puts("thruster actuator original-ELF gold compare: PASS (bitwise)");
    return 0;
}

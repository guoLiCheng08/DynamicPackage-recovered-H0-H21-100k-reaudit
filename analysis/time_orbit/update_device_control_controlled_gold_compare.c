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

static int compare_block(const char *label, const double *actual,
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

static void inertia_update_probe(uint32_t update_flag, void *opaque)
{
    uint32_t *out_flag = opaque;
    *out_flag = update_flag;
}

int main(void)
{
    const uint64_t expected_wheel_omega[4] = {
        UINT64_C(0x3f947ae147ae147b), UINT64_C(0xbfa47ae147ae147b),
        UINT64_C(0x3fa999999999999a), UINT64_C(0xbfa999999999999a)
    };
    const uint64_t expected_wheel_h[4] = {
        UINT64_C(0x3f60624dd2f1a9fc), UINT64_C(0xbf70624dd2f1a9fc),
        UINT64_C(0x3f747ae147ae147c), UINT64_C(0xbf747ae147ae147c)
    };
    const uint64_t expected_wheel_group_h[3] = {
        UINT64_C(0x3f60624dd2f1a9fc), UINT64_C(0xbf70624dd2f1a9fc),
        UINT64_C(0x3f747ae147ae147c)
    };
    const uint64_t expected_wheel_group_torque[3] = {
        UINT64_C(0x3f947ae147ae147b), UINT64_C(0xbfa47ae147ae147b),
        UINT64_C(0x3fa999999999999a)
    };
    const uint64_t expected_mtq_channel[6] = {
        UINT64_C(0x3fd0000000000000), UINT64_C(0xbff0000000000000),
        UINT64_C(0x4000000000000000), UINT64_C(0xc008000000000000),
        UINT64_C(0x0000000000000000), UINT64_C(0x4014000000000000)
    };
    const uint64_t expected_mtq_group[3] = {
        UINT64_C(0x3fd0000000000000), UINT64_C(0xbff0000000000000),
        UINT64_C(0x4000000000000000)
    };
    const uint64_t expected_thruster_force[3] = {
        UINT64_C(0x4036800000000000), UINT64_C(0x4034000000000000),
        UINT64_C(0x4031800000000000)
    };
    const uint64_t expected_thruster_torque[3] = {
        UINT64_C(0xc039000000000000), UINT64_C(0x4049000000000000),
        UINT64_C(0xc039000000000000)
    };
    const uint64_t expected_sada_angle[2] = {
        UINT64_C(0x3f60624dd2f1a9fd), UINT64_C(0xbf60624dd2f1a9fd)
    };
    const uint64_t expected_sada_velocity[2] = {
        UINT64_C(0x3f947ae147ae147c), UINT64_C(0xbf947ae147ae147c)
    };
    const uint64_t expected_sada_acceleration[2] = {
        UINT64_C(0x3fc999999999999b), UINT64_C(0xbfc999999999999b)
    };
    DpReactionWheelRecovered wheels[DP_WHEEL_COUNT];
    DpMtqRecovered mtq[6];
    DpThrusterRecovered thruster;
    DpSadaRecovered sada;
    double wheel_h_data[3] = {0.0};
    double wheel_torque_data[3] = {0.0};
    double wheel_mapping_data[12] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0
    };
    double mtq_channel_data[6] = {0.0};
    double mtq_group_data[3] = {0.0};
    double mtq_mapping_data[18] = {
        1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0, 0.0
    };
    double lever_arm_data[3] = {1.0, 2.0, 3.0};
    double force_input_data[3] = {9.0, 8.0, 7.0};
    double force_output_data[3] = {0.0, 0.0, 0.0};
    double thruster_torque_data[3] = {0.0, 0.0, 0.0};
    DpVector wheel_h = {3, 0, wheel_h_data};
    DpVector wheel_torque = {3, 0, wheel_torque_data};
    DpMatrix wheel_mapping = {3, 4, 4, 0, wheel_mapping_data};
    DpVector mtq_channel = {6, 0, mtq_channel_data};
    DpVector mtq_group = {3, 0, mtq_group_data};
    DpMatrix mtq_mapping = {3, 6, 6, 0, mtq_mapping_data};
    DpDeviceControlCommand command;
    DpDeviceControlContext context;
    uint32_t recorded_inertia_flag = UINT32_MAX;
    unsigned index;

    memset(wheels, 0, sizeof(wheels));
    memset(mtq, 0, sizeof(mtq));
    memset(&thruster, 0, sizeof(thruster));
    memset(&sada, 0, sizeof(sada));
    memset(&command, 0, sizeof(command));
    memset(&context, 0, sizeof(context));
    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        wheels[index].add_gaussian_noise_flag = 0u;
        wheels[index].omega_limit = 1.0;
        wheels[index].torque_limit = 0.05;
        wheels[index].inertia = 0.1;
    }
    mtq[0].moment_limit = 0.5; mtq[1].moment_limit = 1.0;
    mtq[2].moment_limit = 2.0; mtq[3].moment_limit = 3.0;
    mtq[4].moment_limit = 4.0; mtq[5].moment_limit = 5.0;
    thruster.force_scale = 2.5;
    thruster.lever_arm = (DpVector){3, 0, lever_arm_data};
    thruster.force_input = (DpVector){3, 0, force_input_data};
    thruster.force_output = (DpVector){3, 0, force_output_data};
    thruster.torque_output = (DpVector){3, 0, thruster_torque_data};
    sada.command_limit[0] = 1.0; sada.command_limit[1] = 1.0;
    sada.acceleration_limit[0] = 0.2; sada.acceleration_limit[1] = 0.2;

    command.wheel_torque_command[0] = 0.02;
    command.wheel_torque_command[1] = -0.04;
    command.wheel_torque_command[2] = 0.06;
    command.wheel_torque_command[3] = -0.08;
    command.mtq_moment_command[0] = 0.25;
    command.mtq_moment_command[1] = -2.0;
    command.mtq_moment_command[2] = 3.0;
    command.mtq_moment_command[3] = -4.0;
    command.mtq_moment_command[4] = 0.0;
    command.mtq_moment_command[5] = 6.0;
    command.sada_command_flag = 42u;
    command.sada_command_angle[0] = 0.3;
    command.sada_command_angle[1] = -0.4;
    command.thruster_work_status = 1u;
    command.inertia_update_flag = 1u;

    context.wheels = wheels;
    context.wheel_group_angular_momentum_3 = &wheel_h;
    context.wheel_group_torque_3 = &wheel_torque;
    context.wheel_mapping_3x4 = &wheel_mapping;
    context.mtq = mtq;
    context.mtq_group_moment_3 = &mtq_group;
    context.mtq_channel_moment_6 = &mtq_channel;
    context.mtq_mapping_3x6 = &mtq_mapping;
    context.thruster = &thruster;
    context.sada = &sada;
    context.inertia_update = inertia_update_probe;
    context.inertia_update_opaque = &recorded_inertia_flag;

    if (dp_update_device_control(&command, 0.1, &context) != 0 ||
        recorded_inertia_flag != 1u ||
        compare_block("wheel_group_h", wheel_h_data, expected_wheel_group_h, 3u) != 0 ||
        compare_block("wheel_group_torque", wheel_torque_data, expected_wheel_group_torque, 3u) != 0 ||
        compare_block("mtq_channel", mtq_channel_data, expected_mtq_channel, 6u) != 0 ||
        compare_block("mtq_group", mtq_group_data, expected_mtq_group, 3u) != 0 ||
        compare_block("thruster_force", force_output_data, expected_thruster_force, 3u) != 0 ||
        compare_block("thruster_torque", thruster_torque_data, expected_thruster_torque, 3u) != 0 ||
        compare_block("sada_angle", sada.current_angle, expected_sada_angle, 2u) != 0 ||
        compare_block("sada_velocity", sada.angular_velocity, expected_sada_velocity, 2u) != 0 ||
        compare_block("sada_acceleration", sada.angular_acceleration, expected_sada_acceleration, 2u) != 0) return 1;
    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        if (bits(wheels[index].omega) != expected_wheel_omega[index] ||
            bits(wheels[index].angular_momentum) != expected_wheel_h[index]) {
            printf("FAIL wheel[%u] omega_bits=%016" PRIx64 " h_bits=%016" PRIx64 "\n",
                   index, bits(wheels[index].omega), bits(wheels[index].angular_momentum));
            return 1;
        }
    }
    puts("UpdateDeviceControl controlled original-ELF gold compare: PASS (bitwise)");
    return 0;
}

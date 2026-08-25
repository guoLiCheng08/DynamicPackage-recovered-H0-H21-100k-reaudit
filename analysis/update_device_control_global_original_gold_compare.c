#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"
#include "dynamic_satellite_globals.h"

#ifndef GOLD
#define GOLD "analysis/golden/update_device_control_global_abi/controlled_input/"
#endif

_Static_assert(sizeof(DpDeviceControlCommand) == 0x78u,
               "UpdateDeviceControl command ABI must remain 0x78 bytes");

static int read_exact(const char *name, void *dst, size_t bytes)
{
    char path[512];
    FILE *file;
    size_t got;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return -1;
    }
    got = fread(dst, 1u, bytes, file);
    if (fclose(file) != 0 || got != bytes) {
        return -1;
    }
    return 0;
}

static int compare_blob(const char *label, const void *actual, const void *expected,
                        size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;
    unsigned mismatches = 0u;

    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            if (mismatches < 6u) {
                printf("%s byte+0x%zx actual=%02x expected=%02x\n",
                       label, index, (unsigned)a[index], (unsigned)e[index]);
            }
            ++mismatches;
        }
    }
    printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

int main(void)
{
    DpDeviceControlCommand input;
    DpReactionWheelRecovered pre_wheels[DP_WHEEL_COUNT];
    DpMtqRecovered pre_mtq[6];
    DpSadaRecovered pre_sada;
    double pre_wheel_h[3], pre_wheel_torque[3], pre_wheel_map[12];
    double pre_mtq_group[3], pre_mtq_channel[6], pre_mtq_map[18];
    double pre_thruster_scale, pre_thruster_lever[3], pre_thruster_force_input[3];
    double pre_j[9], pre_j_inv[9];
    DpReactionWheelRecovered expected_wheels_post[DP_WHEEL_COUNT];
    DpMtqRecovered expected_mtq_post[6];
    DpSadaRecovered expected_sada_post;
    double expected_wheel_h[3], expected_wheel_torque[3];
    double expected_mtq_group[3], expected_mtq_channel[6];
    double expected_thruster_force[3], expected_thruster_torque[3];
    double expected_j[9], expected_j_inv[9];
    int mismatch = 0;

    if (read_exact("pre_input_78.bin", &input, sizeof(input)) != 0 ||
        read_exact("pre_rwheel_4.bin", pre_wheels, sizeof(pre_wheels)) != 0 ||
        read_exact("pre_mtq_6.bin", pre_mtq, sizeof(pre_mtq)) != 0 ||
        read_exact("pre_sada_104.bin", &pre_sada, sizeof(pre_sada)) != 0 ||
        read_exact("pre_wheel_h_3.bin", pre_wheel_h, sizeof(pre_wheel_h)) != 0 ||
        read_exact("pre_wheel_torque_3.bin", pre_wheel_torque, sizeof(pre_wheel_torque)) != 0 ||
        read_exact("pre_wheel_map_12.bin", pre_wheel_map, sizeof(pre_wheel_map)) != 0 ||
        read_exact("pre_mtq_group_3.bin", pre_mtq_group, sizeof(pre_mtq_group)) != 0 ||
        read_exact("pre_mtq_channel_6.bin", pre_mtq_channel, sizeof(pre_mtq_channel)) != 0 ||
        read_exact("pre_mtq_map_18.bin", pre_mtq_map, sizeof(pre_mtq_map)) != 0 ||
        read_exact("pre_thruster_scale.bin", &pre_thruster_scale, sizeof(pre_thruster_scale)) != 0 ||
        read_exact("pre_thruster_lever_3.bin", pre_thruster_lever, sizeof(pre_thruster_lever)) != 0 ||
        read_exact("pre_thruster_force_input_3.bin", pre_thruster_force_input,
                   sizeof(pre_thruster_force_input)) != 0 ||
        read_exact("pre_j_c_b_9.bin", pre_j, sizeof(pre_j)) != 0 ||
        read_exact("pre_j_c_b_inv_9.bin", pre_j_inv, sizeof(pre_j_inv)) != 0 ||
        read_exact("post_rwheel_4.bin", expected_wheels_post, sizeof(expected_wheels_post)) != 0 ||
        read_exact("post_mtq_6.bin", expected_mtq_post, sizeof(expected_mtq_post)) != 0 ||
        read_exact("post_sada_104.bin", &expected_sada_post, sizeof(expected_sada_post)) != 0 ||
        read_exact("post_wheel_h_3.bin", expected_wheel_h, sizeof(expected_wheel_h)) != 0 ||
        read_exact("post_wheel_torque_3.bin", expected_wheel_torque,
                   sizeof(expected_wheel_torque)) != 0 ||
        read_exact("post_mtq_group_3.bin", expected_mtq_group, sizeof(expected_mtq_group)) != 0 ||
        read_exact("post_mtq_channel_6.bin", expected_mtq_channel,
                   sizeof(expected_mtq_channel)) != 0 ||
        read_exact("post_thruster_force_3.bin", expected_thruster_force,
                   sizeof(expected_thruster_force)) != 0 ||
        read_exact("post_thruster_torque_3.bin", expected_thruster_torque,
                   sizeof(expected_thruster_torque)) != 0 ||
        read_exact("post_j_c_b_9.bin", expected_j, sizeof(expected_j)) != 0 ||
        read_exact("post_j_c_b_inv_9.bin", expected_j_inv, sizeof(expected_j_inv)) != 0) {
        return 1;
    }

    dp_device_globals_reset();
    /* reset 建立 descriptor backing 后，再重放原入口 payload。 */
    memcpy(RWheel, pre_wheels, sizeof(RWheel));
    memcpy(MTQ, pre_mtq, sizeof(MTQ));
    memcpy(&SADA, &pre_sada, sizeof(SADA));
    memcpy(WheelGroup.angular_momentum.data, pre_wheel_h, sizeof(pre_wheel_h));
    memcpy(WheelGroup.torque.data, pre_wheel_torque, sizeof(pre_wheel_torque));
    memcpy(WheelGroup.mapping_3x4.data, pre_wheel_map, sizeof(pre_wheel_map));
    memcpy(MTQ_Group.group_moment.data, pre_mtq_group, sizeof(pre_mtq_group));
    memcpy(MTQ_Group.channel_moment.data, pre_mtq_channel, sizeof(pre_mtq_channel));
    memcpy(MTQ_Group.mapping_3x6.data, pre_mtq_map, sizeof(pre_mtq_map));
    Thruster.force_scale = pre_thruster_scale;
    memcpy(Thruster.lever_arm.data, pre_thruster_lever, sizeof(pre_thruster_lever));
    memcpy(Thruster.force_input.data, pre_thruster_force_input, sizeof(pre_thruster_force_input));
    memcpy(J_c_B_mem, pre_j, sizeof(pre_j));
    memcpy(J_c_B_inv_mem, pre_j_inv, sizeof(pre_j_inv));
    step_time = 0.1;

    UpdateDeviceControl(&input, 0.1);

    mismatch |= compare_blob("RWheel", RWheel, expected_wheels_post, sizeof(RWheel));
    mismatch |= compare_blob("MTQ", MTQ, expected_mtq_post, sizeof(MTQ));
    mismatch |= compare_blob("SADA", &SADA, &expected_sada_post, sizeof(SADA));
    mismatch |= compare_blob("WheelGroup.H", WheelGroup.angular_momentum.data,
                             expected_wheel_h, sizeof(expected_wheel_h));
    mismatch |= compare_blob("WheelGroup.torque", WheelGroup.torque.data,
                             expected_wheel_torque, sizeof(expected_wheel_torque));
    mismatch |= compare_blob("MTQ_Group.moment", MTQ_Group.group_moment.data,
                             expected_mtq_group, sizeof(expected_mtq_group));
    mismatch |= compare_blob("MTQ_Group.channel", MTQ_Group.channel_moment.data,
                             expected_mtq_channel, sizeof(expected_mtq_channel));
    mismatch |= compare_blob("Thruster.force", Thruster.force_output.data,
                             expected_thruster_force, sizeof(expected_thruster_force));
    mismatch |= compare_blob("Thruster.torque", Thruster.torque_output.data,
                             expected_thruster_torque, sizeof(expected_thruster_torque));
    mismatch |= compare_blob("J_c_B", J_c_B_mem, expected_j, sizeof(expected_j));
    mismatch |= compare_blob("J_c_B_inv", J_c_B_inv_mem, expected_j_inv, sizeof(expected_j_inv));
    return mismatch == 0 ? 0 : 1;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_recovered.h"
#include "dynamic_devices.h"
#include "dynamic_satellite_globals.h"
#include "dynamic_sensors.h"
#include "dynamic_ipc_telemetry.h"
#include "dynamic_telemetry_layout.h"

#define GOLD_DIR "analysis/time_orbit/"
#define DYN_GOLD_DIR "analysis/golden/dyn_main_global_abi/"

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    size_t count;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    if (fclose(file) != 0 || count != bytes) {
        return -1;
    }
    return 0;
}

static int read_dyn_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    size_t count;

    (void)snprintf(path, sizeof(path), "%s%s", DYN_GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    if (fclose(file) != 0 || count != bytes) {
        return -1;
    }
    return 0;
}

static int seed_dyn_init_devices(void)
{
    if (read_dyn_blob("pre_rwheel_4.bin", RWheel, sizeof(RWheel)) != 0 ||
        read_dyn_blob("pre_mtq_6.bin", MTQ, sizeof(MTQ)) != 0 ||
        read_dyn_blob("pre_sada_104.bin", &SADA, sizeof(SADA)) != 0 ||
        read_dyn_blob("pre_wheel_h_3.bin", WheelGroup.angular_momentum.data, 24u) != 0 ||
        read_dyn_blob("pre_wheel_torque_3.bin", WheelGroup.torque.data, 24u) != 0 ||
        read_dyn_blob("pre_wheel_map_12.bin", WheelGroup.mapping_3x4.data, 96u) != 0 ||
        read_dyn_blob("pre_mtq_group_3.bin", MTQ_Group.group_moment.data, 24u) != 0 ||
        read_dyn_blob("pre_mtq_channel_6.bin", MTQ_Group.channel_moment.data, 48u) != 0 ||
        read_dyn_blob("pre_mtq_map_18.bin", MTQ_Group.mapping_3x6.data, 144u) != 0 ||
        read_dyn_blob("pre_thruster_b0.bin", &Thruster.force_scale, sizeof(Thruster.force_scale)) != 0 ||
        read_dyn_blob("pre_thruster_lever_3.bin", Thruster.lever_arm.data, 24u) != 0 ||
        read_dyn_blob("pre_thruster_force_input_3.bin", Thruster.force_input.data, 24u) != 0 ||
        read_dyn_blob("pre_thruster_force_output_3.bin", Thruster.force_output.data, 24u) != 0 ||
        read_dyn_blob("pre_thruster_torque_output_3.bin", Thruster.torque_output.data, 24u) != 0) {
        return -1;
    }
    return 0;
}

static int dump_sat_matrix(FILE *file, size_t offset, size_t bytes)
{
    DpMatrix matrix;

    memcpy(&matrix, &Sat.raw[offset], sizeof(matrix));
    return matrix.data == NULL || fwrite(matrix.data, 1u, bytes, file) != bytes ? -1 : 0;
}

static int dump_sada_model(const char *path)
{
    FILE *file = fopen(path, "wb");
    int status = -1;

    if (file == NULL) {
        return -1;
    }
    if (fwrite(J_c_B_mem, 1u, sizeof(J_c_B_mem), file) == sizeof(J_c_B_mem) &&
        fwrite(J_c_B_inv_mem, 1u, sizeof(J_c_B_inv_mem), file) == sizeof(J_c_B_inv_mem) &&
        dump_sat_matrix(file, 0x3e8u, 72u) == 0 &&
        dump_sat_matrix(file, 0x448u, 72u) == 0 &&
        dump_sat_matrix(file, 0x4a8u, 72u) == 0 &&
        dump_sat_matrix(file, 0x568u, 72u) == 0 &&
        dump_sat_matrix(file, 0x688u, 240u) == 0 &&
        dump_sat_matrix(file, 0x898u, 240u) == 0 &&
        dump_sat_matrix(file, 0x9a0u, 72u) == 0 &&
        dump_sat_matrix(file, 0xa00u, 800u) == 0 &&
        dump_sat_matrix(file, 0xd38u, 800u) == 0) {
        status = 0;
    }
    if (fclose(file) != 0) {
        status = -1;
    }
    return status;
}

static int compare_blob(const char *label, const void *actual, const void *expected,
                        size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;
    unsigned count = 0u;

    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            if (count < 8u) {
                printf("%s byte+0x%zx actual=%02x expected=%02x\n", label, index,
                       (unsigned)a[index], (unsigned)e[index]);
            }
            ++count;
        }
    }
    printf("%s mismatched bytes: %u/%zu\n", label, count, bytes);
    return count == 0u ? 0 : -1;
}

int main(void)
{
    DpInitialConditions initial = {0};
    unsigned char core_output[0x148] = {0};
    DpMainTelemetryFrame main_output = {{0}};
    DpDeviceControlCommand command = {0};
    DpState initial_state = {0};
    unsigned char expected_core_prefix[33u * sizeof(double)];
    unsigned char expected_core_prefix_step2[33u * sizeof(double)];
    DpMainTelemetryFrame expected_main;
    DpMainTelemetryFrame expected_main_step2;
    DpIpcSharedFrame actual_ipc = {{0}};
    DpIpcSharedFrame expected_ipc_step1;
    DpIpcSharedFrame expected_ipc_step2;
    DpSadaRecovered expected_sada_step1;
    DpSadaRecovered expected_sada_step2;
    int mismatch = 0;

    initial.step_time = 0.1;
    initial.initial_time_or_epoch = 1.0;
    initial.initial_angular_rate_f32[0] = 0.001f;
    initial.initial_angular_rate_f32[1] = -0.002f;
    initial.initial_angular_rate_f32[2] = 0.003f;
    initial.time_parameters[0] = 2025.0;
    initial.time_parameters[1] = 1.0;
    initial.time_parameters[2] = 2.0;
    initial.time_parameters[3] = 3.0;
    initial.time_parameters[4] = 4.0;
    initial.time_parameters[5] = 5.0;
    initial.orbit_elements[0] = 7000000.0;
    initial.orbit_elements[1] = 0.01;
    initial.orbit_elements[2] = 0.5;
    initial.orbit_elements[3] = 0.2;
    initial.orbit_elements[4] = 0.3;
    initial.orbit_elements[5] = 0.4;
    initial.inertia_tensor[0] = 120.0;
    initial.inertia_tensor[4] = 100.0;
    initial.inertia_tensor[8] = 80.0;
    initial.spacecraft_mass = 1000.0;

    if (read_blob("gold_sensor_init_gyro.bin", DeviceMeasure.gyro,
                  sizeof(DeviceMeasure.gyro)) != 0 ||
        read_blob("gold_sensor_init_magmeter.bin", DeviceMeasure.magmeter,
                  sizeof(DeviceMeasure.magmeter)) != 0 ||
        read_blob("gold_sensor_init_sts.bin", DeviceMeasure.sts,
                  sizeof(DeviceMeasure.sts)) != 0 ||
        read_blob("gold_sensor_init_dss.bin", DeviceMeasure.dss,
                  sizeof(DeviceMeasure.dss)) != 0 ||
        read_blob("gold_sada_position_step1_state.bin", expected_core_prefix,
                  sizeof(expected_core_prefix)) != 0 ||
        read_blob("gold_sada_position_step1_out.bin", &expected_main,
                  sizeof(expected_main)) != 0 ||
        read_blob("gold_sada_position_step2_state.bin", expected_core_prefix_step2,
                  sizeof(expected_core_prefix_step2)) != 0 ||
        read_blob("gold_sada_position_step2_out.bin", &expected_main_step2,
                  sizeof(expected_main_step2)) != 0 ||
        read_blob("gold_sada_position_step1_ipc.bin", &expected_ipc_step1,
                  sizeof(expected_ipc_step1)) != 0 ||
        read_blob("gold_sada_position_step2_ipc.bin", &expected_ipc_step2,
                  sizeof(expected_ipc_step2)) != 0 ||
        read_blob("gold_sada_position_step1_sada.bin", &expected_sada_step1,
                  sizeof(expected_sada_step1)) != 0 ||
        read_blob("gold_sada_position_step2_sada.bin", &expected_sada_step2,
                  sizeof(expected_sada_step2)) != 0) {
        return 1;
    }
    dp_device_measure_globals_relocate();
    DeviceMeasure.gps_init_flag = 0;
    /* 原探针调用 dyn_init：DynamicInit 之前已完成 Wheel/MTQ/Thruster 等 descriptor 初始化。 */
    dp_device_globals_reset();
    if (seed_dyn_init_devices() != 0) {
        return 1;
    }
    DynamicInit(&initial);
    command.sada_command_flag = 1u;
    command.sada_command_angle[0] = 0.01;
    command.sada_command_angle[1] = -0.01;
    {
        FILE *file = fopen("/tmp/dyn_main_c_pre_y.bin", "wb");
        if (file == NULL || fwrite(y, 1u, sizeof(y), file) != sizeof(y) ||
            (file != NULL && fclose(file) != 0)) {
            return 1;
        }
    }
    initial_state.attitude_q.q0 = 1.0;
    initial_state.body_rate.x = 0.001;
    initial_state.body_rate.y = -0.002;
    initial_state.body_rate.z = 0.003;
    initial_state.position_gci.x = 7000000.0;
    initial_state.velocity_gci.y = 7546.0;
    memcpy(core_output, &initial_state, sizeof(initial_state));
    srand(1u);

    dyn_main(&main_output, core_output, &command);
    {
        FILE *file = fopen("/tmp/dyn_main_global_step1_core_actual.bin", "wb");
        if (file == NULL || fwrite(core_output, 1u, sizeof(core_output), file) != sizeof(core_output) ||
            (file != NULL && fclose(file) != 0)) {
            return 1;
        }
        file = fopen("/tmp/dyn_main_global_step1_main_actual.bin", "wb");
        if (file == NULL || fwrite(main_output.raw, 1u, sizeof(main_output.raw), file) !=
                              sizeof(main_output.raw) ||
            (file != NULL && fclose(file) != 0)) {
            return 1;
        }
    }

    {
        FILE *file = fopen("/tmp/dyn_main_sada_step1_actual.bin", "wb");
        if (file == NULL || fwrite(&SADA, 1u, sizeof(SADA), file) != sizeof(SADA) ||
            (file != NULL && fclose(file) != 0)) {
            return 1;
        }
    }
    /* Sat 的未映射 descriptor 槽位仅用于诊断导出，不能作为本顶层行为金标的
     * 前置条件；实际验收覆盖 CoreDynamic、遥测、SADA 设备对象与 IPC。 */
    (void)dump_sada_model("/tmp/dyn_main_sada_step1_model_actual.bin");
    mismatch |= compare_blob("dyn_main step1 CoreDynamic first 0x108", core_output,
                             expected_core_prefix, sizeof(expected_core_prefix));
    mismatch |= compare_blob("dyn_main step1 main output", main_output.raw,
                             expected_main.raw, sizeof(main_output.raw));
    mismatch |= compare_blob("dyn_main step1 SADA", &SADA, &expected_sada_step1,
                             sizeof(SADA));
    dp_send_dyn_tele(&actual_ipc, &main_output, 0u, 0u, 0u, 0u, 0u);
    mismatch |= compare_blob("dyn_main step1 IPC payload", actual_ipc.raw + DP_IPC_FLOAT_BASE,
                             expected_ipc_step1.raw + DP_IPC_FLOAT_BASE,
                             DP_IPC_SHM_BYTES - DP_IPC_FLOAT_BASE);

    dyn_main(&main_output, core_output, &command);
    mismatch |= compare_blob("dyn_main step2 CoreDynamic first 0x108", core_output,
                             expected_core_prefix_step2, sizeof(expected_core_prefix_step2));
    mismatch |= compare_blob("dyn_main step2 main output", main_output.raw,
                             expected_main_step2.raw, sizeof(main_output.raw));
    mismatch |= compare_blob("dyn_main step2 SADA", &SADA, &expected_sada_step2,
                             sizeof(SADA));
    memset(&actual_ipc, 0, sizeof(actual_ipc));
    dp_send_dyn_tele(&actual_ipc, &main_output, 0u, 0u, 0u, 0u, 0u);
    mismatch |= compare_blob("dyn_main step2 IPC payload", actual_ipc.raw + DP_IPC_FLOAT_BASE,
                             expected_ipc_step2.raw + DP_IPC_FLOAT_BASE,
                             DP_IPC_SHM_BYTES - DP_IPC_FLOAT_BASE);
    if (mismatch == 0) {
        puts("dyn_main SADA-position two-step original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

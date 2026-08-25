#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "dynamic_main_bridge.h"
#include "dynamic_recovered.h"
#include "dynamic_time.h"

#define GOLD_DIR "analysis/time_orbit/"

#ifdef E8_COUNT_RAND
extern int e8_rand_stage;
static unsigned e8_gaussian2_calls = 0u;
static double e8_counting_gaussian2(double mean, double sigma, void *opaque)
{
    double x;
    double y;
    double radius_squared;
    (void)opaque;
    ++e8_gaussian2_calls;
    do {
        do { x = (double)rand() / 2147483647.0; } while (x == 0.0 || x == 1.0);
        x = x + x;
        x = x - 1.0;
        do { y = (double)rand() / 2147483647.0; } while (y == 0.0 || y == 1.0);
        y = y + y;
        y = y - 1.0;
        radius_squared = x * x;
        radius_squared += y * y;
    } while (radius_squared > 1.0 || radius_squared == 0.0);
    y *= sigma;
    return y * sqrt((-2.0 * log(radius_squared)) / radius_squared) + mean;
}
#endif

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) { perror(path); return -1; }
    if (fread(out, 1u, bytes, file) != bytes || fclose(file) != 0) return -1;
    return 0;
}

static int compare_bytes(const char *label, const void *actual, const void *expected,
                         size_t bytes)
{
    const uint8_t *actual_bytes = actual;
    const uint8_t *expected_bytes = expected;
    size_t index;
    for (index = 0u; index < bytes; ++index) {
        if (actual_bytes[index] != expected_bytes[index]) {
            fprintf(stderr, "%s +0x%zx got=%02x expected=%02x\n", label, index,
                    (unsigned)actual_bytes[index], (unsigned)expected_bytes[index]);
            return -1;
        }
    }
    return 0;
}

static void relocate_devices(DpDeviceMeasureRecovered *devices)
{
    unsigned index;
    for (index = 0u; index < 2u; ++index) {
        const size_t item = (size_t)index * 0x320u;
        devices->gyro[index].projection_matrix.data =
            (double *)((uint8_t *)devices->gyro + item + 0xc8u);
        devices->gyro[index].measure.data =
            (double *)((uint8_t *)devices->gyro + item + 0x128u);
    }
    for (index = 0u; index < 2u; ++index) {
        const size_t item = (size_t)index * 0x138u;
        devices->magmeter[index].projection_matrix.data =
            (double *)((uint8_t *)devices->magmeter + item + 0xc0u);
        devices->magmeter[index].measure.data =
            (double *)((uint8_t *)devices->magmeter + item + 0x120u);
    }
    for (index = 0u; index < 3u; ++index) {
        const size_t item = (size_t)index * 0x170u;
        devices->sts[index].installation_matrix.data =
            (double *)((uint8_t *)devices->sts + item + 0x78u);
        devices->sts[index].error_quat.xyz.data =
            (double *)((uint8_t *)devices->sts + item + 0x118u);
        devices->sts[index].measure_quat.xyz.data =
            (double *)((uint8_t *)devices->sts + item + 0x158u);
    }
    for (index = 0u; index < 2u; ++index) {
        const size_t item = (size_t)index * 0x158u;
        devices->dss[index].projection_matrix.data =
            (double *)((uint8_t *)devices->dss + item + 0xb8u);
        devices->dss[index].measure.data =
            (double *)((uint8_t *)devices->dss + item + 0x120u);
    }
}

static void init_minimal_thruster(DpThrusterRecovered *thruster, double lever[3],
                                  double force_in[3], double force_out[3], double torque[3])
{
    memset(thruster, 0, sizeof(*thruster));
    thruster->lever_arm = (DpVector){3, 0, lever};
    thruster->force_input = (DpVector){3, 0, force_in};
    thruster->force_output = (DpVector){3, 0, force_out};
    thruster->torque_output = (DpVector){3, 0, torque};
}

int main(void)
{
    DpCoreDefaultModel model;
    DpCoreDefaultModel warmup_model;
    DpDeviceMeasureRecovered devices;
    DpDeviceMeasureRecovered warmup_devices;
    DpReactionWheelRecovered wheels[DP_WHEEL_COUNT];
    DpReactionWheelRecovered warmup_wheels[DP_WHEEL_COUNT];
    DpMtqRecovered mtq[6];
    DpSadaRecovered sada;
    DpThrusterRecovered thruster;
    double wheel_h_data[3] = {0.0};
    double wheel_torque_data[3] = {0.0};
    double wheel_map_data[12] = {0.0};
    double mtq_group_data[3] = {0.0};
    double mtq_channel_data[6] = {0.0};
    double mtq_map_data[18] = {0.0};
    double lever_data[3] = {0.0};
    double force_in_data[3] = {0.0};
    double force_out_data[3] = {0.0};
    double thruster_torque_data[3] = {0.0};
    DpVector wheel_h = {3, 0, wheel_h_data};
    DpVector wheel_torque = {3, 0, wheel_torque_data};
    DpMatrix wheel_map = {3, 4, 4, 0, wheel_map_data};
    DpVector mtq_group = {3, 0, mtq_group_data};
    DpVector mtq_channel = {6, 0, mtq_channel_data};
    DpMatrix mtq_map = {3, 6, 6, 0, mtq_map_data};
    DpDeviceControlContext device_control;
    DpDynMainRecoveredContext context;
    DpDeviceControlCommand command;
    DpDynMainCoreTerms core_terms;
    DpDynMainIpcControl ipc_control;
    DpState state;
    DpState warmup_state;
    DpCoreEnvironmentOutputs warmup_environment;
    DpTelemetrySourceSnapshot warmup_source;
    DpMainTelemetryFrame warmup_out;
    DpMainTelemetryFrame actual_out;
    DpMainTelemetryFrame expected_out;
    DpState expected_state;
    DpIpcSharedFrame actual_ipc;
    DpIpcSharedFrame expected_ipc;
    double integration_time = 0.1;

    memset(&devices, 0, sizeof(devices));
    memset(&device_control, 0, sizeof(device_control));
    memset(&context, 0, sizeof(context));
    memset(&command, 0, sizeof(command));
    memset(&core_terms, 0, sizeof(core_terms));
    memset(&ipc_control, 0, sizeof(ipc_control));
    memset(&actual_out, 0, sizeof(actual_out));
    memset(&actual_ipc, 0, sizeof(actual_ipc));
    if (read_blob("gold_sada_position_step1_post_y.bin", &state, sizeof(state)) != 0 ||
        read_blob("gold_sada_position_step1_sts.bin", devices.sts, sizeof(devices.sts)) != 0 ||
        read_blob("gold_sada_position_step1_gyro.bin", devices.gyro, sizeof(devices.gyro)) != 0 ||
        read_blob("gold_sada_position_step1_dss.bin", devices.dss, sizeof(devices.dss)) != 0 ||
        read_blob("gold_sada_position_step1_magmeter.bin", devices.magmeter,
                  sizeof(devices.magmeter)) != 0 ||
        read_blob("gold_sada_position_step1_gps.bin", &devices.gps, sizeof(devices.gps)) != 0 ||
        read_blob("gold_sada_position_step1_wheels.bin", wheels, sizeof(wheels)) != 0 ||
        read_blob("gold_sada_position_step1_sada.bin", &sada, sizeof(sada)) != 0 ||
        read_blob("gold_full_dyn_init_mtq.bin", mtq, sizeof(mtq)) != 0 ||
        read_blob("gold_sada_position_step2_post_y.bin", &expected_state, sizeof(expected_state)) != 0 ||
        read_blob("gold_sada_position_step2_out.bin", &expected_out, sizeof(expected_out)) != 0 ||
        read_blob("gold_sada_position_step2_ipc.bin", &expected_ipc, sizeof(expected_ipc)) != 0 ||
        read_blob("gold_full_dyn_init_y.bin", &warmup_state, sizeof(warmup_state)) != 0 ||
        read_blob("gold_full_dyn_init_sts.bin", warmup_devices.sts,
                  sizeof(warmup_devices.sts)) != 0 ||
        read_blob("gold_full_dyn_init_gyro.bin", warmup_devices.gyro,
                  sizeof(warmup_devices.gyro)) != 0 ||
        read_blob("gold_full_dyn_init_dss.bin", warmup_devices.dss,
                  sizeof(warmup_devices.dss)) != 0 ||
        read_blob("gold_full_dyn_init_magmeter.bin", warmup_devices.magmeter,
                  sizeof(warmup_devices.magmeter)) != 0 ||
        read_blob("gold_full_dyn_init_wheels.bin", warmup_wheels,
                  sizeof(warmup_wheels)) != 0 ||
        dp_core_default_model_init(&model) != 0 ||
        dp_core_default_model_init(&warmup_model) != 0) return 1;
    relocate_devices(&devices);
    relocate_devices(&warmup_devices);
    srand(1u);
    TimeInit(2025.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    SpacecraftMass = 1000.0;
    F_I_external.data[0] = 0.0;
    F_I_external.data[1] = 0.0;
    F_I_external.data[2] = 0.0;
    if (dp_core_default_rk4_step_with_terms_and_environment(&warmup_model,
            (double *)&warmup_state, &(double){0.0}, 0.1, NULL, NULL,
            &warmup_environment) != 0) return 1;
    dp_update_device_measure_recovered(&warmup_devices, &warmup_state,
        &warmup_environment.device_environment, 0.1, NULL, NULL);
    if (dp_telemetry_source_from_devices(&warmup_source, &warmup_devices) != 0) return 1;
    dp_update_main_out_pack(&warmup_out, &warmup_source);
    /* 原第一轮 UpdateMainOut 在星敏噪声之后逐轮调用 GetWheelSpeed；该阶段
     * 的四次随机读取必须在从第一轮设备快照启动第二轮前重放。 */
    { unsigned wheel_index; for (wheel_index = 0u; wheel_index < DP_WHEEL_COUNT; ++wheel_index)
        (void)dp_get_wheel_speed(warmup_wheels, wheel_index); }
    {
        FILE *debug_file = fopen("/tmp/e8_recovered_step1_warmup_out.bin", "wb");
        if (debug_file == NULL || fwrite(&warmup_out, 1u, sizeof(warmup_out), debug_file) !=
            sizeof(warmup_out) || fclose(debug_file) != 0) return 1;
    }
#ifdef E8_COUNT_RAND
    e8_rand_stage = 1;
#endif
    command.sada_command_flag = 1u;
    command.sada_command_angle[0] = 0.01;
    command.sada_command_angle[1] = -0.01;
    init_minimal_thruster(&thruster, lever_data, force_in_data, force_out_data,
                          thruster_torque_data);
    device_control.wheels = wheels;
    device_control.wheel_group_angular_momentum_3 = &wheel_h;
    device_control.wheel_group_torque_3 = &wheel_torque;
    device_control.wheel_mapping_3x4 = &wheel_map;
    device_control.mtq = mtq;
    device_control.mtq_group_moment_3 = &mtq_group;
    device_control.mtq_channel_moment_6 = &mtq_channel;
    device_control.mtq_mapping_3x6 = &mtq_map;
    device_control.thruster = &thruster;
    device_control.sada = &sada;
    context.core_model = &model;
    context.devices = &devices;
    context.device_control = &device_control;
    context.integration_time = &integration_time;
    context.step_time = 0.1;
    context.device_control_step_time = 0.01;
#ifdef E8_COUNT_RAND
    context.gaussian2_sampler = e8_counting_gaussian2;
#endif
    TimeInit(2025.0, 1.0, 2.0, 3.0, 4.0, 5.1);
    SpacecraftMass = 1000.0;
    F_I_external.data[0] = 0.0;
    F_I_external.data[1] = 0.0;
    F_I_external.data[2] = 0.0;
    if (dp_dyn_main_recovered_step(&context, &state, &command, &core_terms, &actual_out,
                                   &actual_ipc, &ipc_control) != 0) return 1;
    {
        FILE *debug_file = fopen("/tmp/e10_recovered_sada_step2_sada.bin", "wb");
        if (debug_file == NULL || fwrite(&sada, 1u, sizeof(sada), debug_file) !=
            sizeof(sada) || fclose(debug_file) != 0) return 1;
    }
    {
        FILE *debug_file = fopen("/tmp/e8_recovered_step2_sts.bin", "wb");
        if (debug_file == NULL || fwrite(devices.sts, 1u, sizeof(devices.sts), debug_file) !=
            sizeof(devices.sts) || fclose(debug_file) != 0) return 1;
    }
    {
        FILE *debug_file = fopen("/tmp/e8_recovered_step2_state.bin", "wb");
        if (debug_file == NULL || fwrite(&state, 1u, sizeof(state), debug_file) !=
            sizeof(state) || fclose(debug_file) != 0) return 1;
    }
    {
        FILE *debug_file = fopen("/tmp/e8_recovered_step2_out.bin", "wb");
        if (debug_file == NULL || fwrite(&actual_out, 1u, sizeof(actual_out), debug_file) !=
            sizeof(actual_out) || fclose(debug_file) != 0) return 1;
    }
#ifdef E8_COUNT_RAND
    (void)fprintf(stderr, "recovered UpdateDeviceMeasure gaussian2 calls=%u\\n", e8_gaussian2_calls);
#endif
    if (compare_bytes("sada_position state", &state, &expected_state, sizeof(state)) != 0 ||
        compare_bytes("sada_position frame", &actual_out, &expected_out, sizeof(actual_out)) != 0 ||
        compare_bytes("dyn_main IPC payload", actual_ipc.raw + DP_IPC_RWLOCK_BYTES,
                      expected_ipc.raw + DP_IPC_RWLOCK_BYTES,
                      DP_IPC_SHM_BYTES - DP_IPC_RWLOCK_BYTES) != 0) return 1;
    puts("recovered dyn_main sada_position second-step original-ELF compare: PASS (state/frame/IPC bitwise)");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_devices.h"
#include "dynamic_ipc_telemetry.h"
#include "dynamic_recovered.h"
#include "dynamic_satellite_globals.h"
#include "dynamic_sensors.h"
#include "dynamic_telemetry_layout.h"

#define GOLD_DIR "analysis/time_orbit/"
#define DYN_GOLD_DIR "analysis/golden/dyn_main_global_abi/"
#define STEP_COUNT 100u
#define STATE_BYTES (33u * sizeof(double))
#define MAIN_BYTES 0x220u
#define IPC_PAYLOAD_BYTES (DP_IPC_SHM_BYTES - DP_IPC_FLOAT_BASE)

static int read_blob_from(const char *directory, const char *name, void *out, size_t bytes)
{
    char path[512];
    FILE *file;
    size_t count;

    (void)snprintf(path, sizeof(path), "%s%s", directory, name);
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

static int read_gold(const char *name, void *out, size_t bytes)
{
    return read_blob_from(GOLD_DIR, name, out, bytes);
}

static int read_dyn_gold(const char *name, void *out, size_t bytes)
{
    return read_blob_from(DYN_GOLD_DIR, name, out, bytes);
}

static int seed_dyn_init_devices(void)
{
    return read_dyn_gold("pre_rwheel_4.bin", RWheel, sizeof(RWheel)) ||
           read_dyn_gold("pre_mtq_6.bin", MTQ, sizeof(MTQ)) ||
           read_dyn_gold("pre_sada_104.bin", &SADA, sizeof(SADA)) ||
           read_dyn_gold("pre_wheel_h_3.bin", WheelGroup.angular_momentum.data, 24u) ||
           read_dyn_gold("pre_wheel_torque_3.bin", WheelGroup.torque.data, 24u) ||
           read_dyn_gold("pre_wheel_map_12.bin", WheelGroup.mapping_3x4.data, 96u) ||
           read_dyn_gold("pre_mtq_group_3.bin", MTQ_Group.group_moment.data, 24u) ||
           read_dyn_gold("pre_mtq_channel_6.bin", MTQ_Group.channel_moment.data, 48u) ||
           read_dyn_gold("pre_mtq_map_18.bin", MTQ_Group.mapping_3x6.data, 144u) ||
           read_dyn_gold("pre_thruster_b0.bin", &Thruster.force_scale, sizeof(Thruster.force_scale)) ||
           read_dyn_gold("pre_thruster_lever_3.bin", Thruster.lever_arm.data, 24u) ||
           read_dyn_gold("pre_thruster_force_input_3.bin", Thruster.force_input.data, 24u) ||
           read_dyn_gold("pre_thruster_force_output_3.bin", Thruster.force_output.data, 24u) ||
           read_dyn_gold("pre_thruster_torque_output_3.bin", Thruster.torque_output.data, 24u);
}

static int compare_blob(const char *label, const void *actual, const void *expected, size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;
    unsigned mismatches = 0u;

    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            if (mismatches < 8u) {
                printf("%s byte+0x%zx actual=%02x expected=%02x\n", label, index,
                       (unsigned)a[index], (unsigned)e[index]);
            }
            ++mismatches;
        }
    }
    printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

static void setup_initial(DpInitialConditions *initial)
{
    memset(initial, 0, sizeof(*initial));
    initial->step_time = 0.1;
    initial->initial_time_or_epoch = 1.0;
    initial->initial_angular_rate_f32[0] = 0.001f;
    initial->initial_angular_rate_f32[1] = -0.002f;
    initial->initial_angular_rate_f32[2] = 0.003f;
    initial->time_parameters[0] = 2025.0;
    initial->time_parameters[1] = 1.0;
    initial->time_parameters[2] = 2.0;
    initial->time_parameters[3] = 3.0;
    initial->time_parameters[4] = 4.0;
    initial->time_parameters[5] = 5.0;
    initial->orbit_elements[0] = 7000000.0;
    initial->orbit_elements[1] = 0.01;
    initial->orbit_elements[2] = 0.5;
    initial->orbit_elements[3] = 0.2;
    initial->orbit_elements[4] = 0.3;
    initial->orbit_elements[5] = 0.4;
    initial->inertia_tensor[0] = 120.0;
    initial->inertia_tensor[4] = 100.0;
    initial->inertia_tensor[8] = 80.0;
    initial->spacecraft_mass = 1000.0;
}

int main(void)
{
    DpInitialConditions initial;
    DpState initial_state = {0};
    DpDeviceControlCommand command = {0};
    unsigned char core_output[0x148] = {0};
    DpMainTelemetryFrame main_output = {{0}};
    DpIpcSharedFrame actual_ipc = {{0}};
    unsigned char *expected_states = NULL;
    unsigned char *expected_main = NULL;
    unsigned char *expected_ipc = NULL;
    unsigned char *expected_global_y = NULL;
    unsigned step;
    int mismatch = 0;

    expected_states = malloc(STEP_COUNT * STATE_BYTES);
    expected_main = malloc(STEP_COUNT * MAIN_BYTES);
    expected_ipc = malloc(STEP_COUNT * IPC_PAYLOAD_BYTES);
    expected_global_y = malloc(STEP_COUNT * STATE_BYTES);
    if (expected_states == NULL || expected_main == NULL || expected_ipc == NULL ||
        expected_global_y == NULL ||
        read_gold("gold_sensor_init_gyro.bin", DeviceMeasure.gyro, sizeof(DeviceMeasure.gyro)) ||
        read_gold("gold_sensor_init_magmeter.bin", DeviceMeasure.magmeter, sizeof(DeviceMeasure.magmeter)) ||
        read_gold("gold_sensor_init_sts.bin", DeviceMeasure.sts, sizeof(DeviceMeasure.sts)) ||
        read_gold("gold_sensor_init_dss.bin", DeviceMeasure.dss, sizeof(DeviceMeasure.dss)) ||
        read_gold("gold_dss0_noise_hundred_step_state.bin", expected_states, STEP_COUNT * STATE_BYTES) ||
        read_gold("gold_dss0_noise_hundred_step_out.bin", expected_main, STEP_COUNT * MAIN_BYTES) ||
        read_gold("gold_dss0_noise_hundred_step_ipc_payload.bin", expected_ipc, STEP_COUNT * IPC_PAYLOAD_BYTES) ||
        read_gold("gold_dss0_noise_h25_global_y_hundred_first_global_y.bin", expected_global_y,
                  STEP_COUNT * STATE_BYTES)) {
        free(expected_states);
        free(expected_main);
        free(expected_ipc);
        free(expected_global_y);
        return 1;
    }

    dp_device_measure_globals_relocate();
    DeviceMeasure.gps_init_flag = 0;
    dp_device_globals_reset();
    if (seed_dyn_init_devices() != 0) {
        free(expected_states);
        free(expected_main);
        free(expected_ipc);
        free(expected_global_y);
        return 1;
    }
    setup_initial(&initial);
    DynamicInit(&initial);
    DeviceMeasure.dss[0].gaussian_noise_flag = 1u;
    DeviceMeasure.dss[1].gaussian_noise_flag = 0u;
    initial_state.attitude_q.q0 = 1.0;
    initial_state.body_rate.x = 0.001;
    initial_state.body_rate.y = -0.002;
    initial_state.body_rate.z = 0.003;
    initial_state.position_gci.x = 7000000.0;
    initial_state.velocity_gci.y = 7546.0;
    memcpy(core_output, &initial_state, sizeof(initial_state));
    srand(1u);

    for (step = 0u; step < STEP_COUNT; ++step) {
        char label[96];
        dyn_main(&main_output, core_output, &command);
        (void)snprintf(label, sizeof(label), "DSS0-noise hundred-step[%u] CoreDynamic first 0x108", step + 1u);
        mismatch |= compare_blob(label, core_output, expected_states + step * STATE_BYTES, STATE_BYTES);
        (void)snprintf(label, sizeof(label), "DSS0-noise hundred-step[%u] main output", step + 1u);
        mismatch |= compare_blob(label, main_output.raw, expected_main + step * MAIN_BYTES, MAIN_BYTES);
        memset(&actual_ipc, 0, sizeof(actual_ipc));
        dp_send_dyn_tele(&actual_ipc, &main_output, 0u, 0u, 0u, 0u, 0u);
        (void)snprintf(label, sizeof(label), "DSS0-noise hundred-step[%u] IPC payload", step + 1u);
        mismatch |= compare_blob(label, actual_ipc.raw + DP_IPC_FLOAT_BASE,
                                 expected_ipc + step * IPC_PAYLOAD_BYTES, IPC_PAYLOAD_BYTES);
        (void)snprintf(label, sizeof(label), "H25 DSS global-y hundred-step[%u] global y", step + 1u);
        mismatch |= compare_blob(label, y, expected_global_y + step * STATE_BYTES, STATE_BYTES);
    }

    free(expected_states);
    free(expected_main);
    free(expected_ipc);
    free(expected_global_y);
    if (mismatch == 0) {
        puts("dyn_main DSS0-noise hundred-step original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

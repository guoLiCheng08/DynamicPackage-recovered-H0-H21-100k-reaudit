#include <stdio.h>
#include <stdint.h>
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
#define STEP_COUNT 10000u
#define STATE_GOLD "gold_cross_flex_dss_altseed_command_ten_thousand_step_state.bin"
#define GLOBAL_Y_GOLD "gold_cross_flex_dss_altseed_command_ten_thousand_step_global_y.bin"
#define MAIN_GOLD "gold_cross_flex_dss_altseed_command_ten_thousand_step_out.bin"
#define IPC_GOLD "gold_cross_flex_dss_altseed_command_ten_thousand_step_ipc_payload.bin"
#define SCENARIO_LABEL "H21 cross-flex DSS alternate-seed command ten-thousand-step"
#define SCENARIO_SEED 12345u
#define COMMAND_LCG_SEED UINT32_C(0x2468ace1)
#define COMMAND_LCG_A UINT32_C(1664525)
#define COMMAND_LCG_C UINT32_C(1013904223)
#define STATE_BYTES (33u * sizeof(double))
#define GLOBAL_Y_BYTES (33u * sizeof(double))
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
    return fclose(file) != 0 || count != bytes ? -1 : 0;
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

static void setup_command(DpDeviceControlCommand *command, uint32_t *command_rng)
{
    uint32_t rng;
    unsigned wheel0;

    memset(command, 0, sizeof(*command));
    *command_rng = *command_rng * COMMAND_LCG_A + COMMAND_LCG_C;
    rng = *command_rng;
    wheel0 = (unsigned)(rng & UINT32_C(3));
    command->wheel_torque_command[wheel0] = (rng & UINT32_C(4)) != 0u ? -0.005 : 0.005;
    if ((rng & UINT32_C(8)) != 0u) {
        unsigned wheel1 = (unsigned)((rng >> 4u) & UINT32_C(3));
        command->wheel_torque_command[wheel1] = (rng & UINT32_C(0x40)) != 0u ? -0.003 : 0.003;
    }
    if ((rng & UINT32_C(0x80)) != 0u) {
        unsigned mtq = (unsigned)((rng >> 8u) % UINT32_C(6));
        command->mtq_moment_command[mtq] = (rng & UINT32_C(0x800)) != 0u ? -0.008 : 0.008;
    }
    if ((rng & UINT32_C(0x1000)) != 0u) {
        command->sada_command_flag = 1u;
        command->sada_command_angle[0] = (rng & UINT32_C(0x2000)) != 0u ? -0.01 : 0.01;
        command->sada_command_angle[1] = (rng & UINT32_C(0x4000)) != 0u ? -0.005 : 0.005;
    }
    if ((rng & UINT32_C(0x8000)) != 0u) {
        command->thruster_work_status = 1u;
    }
    if ((rng & UINT32_C(0x10000)) != 0u) {
        command->inertia_update_flag = 1u;
    }
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
    unsigned char *expected_global_y = NULL;
    unsigned char *expected_main = NULL;
    unsigned char *expected_ipc = NULL;
    unsigned char expected_rwheel[sizeof(RWheel)];
    unsigned char expected_mtq[sizeof(MTQ)];
    unsigned char expected_thruster[sizeof(Thruster)];
    unsigned char expected_thruster_lever[3u * sizeof(double)];
    unsigned char expected_thruster_input[3u * sizeof(double)];
    unsigned char expected_thruster_force[3u * sizeof(double)];
    unsigned char expected_thruster_torque[3u * sizeof(double)];
    uint32_t command_rng = COMMAND_LCG_SEED;
    unsigned step;
    int mismatch = 0;

    expected_states = malloc(STEP_COUNT * STATE_BYTES);
    expected_global_y = malloc(STEP_COUNT * GLOBAL_Y_BYTES);
    expected_main = malloc(STEP_COUNT * MAIN_BYTES);
    expected_ipc = malloc(STEP_COUNT * IPC_PAYLOAD_BYTES);
    if (expected_states == NULL || expected_global_y == NULL || expected_main == NULL || expected_ipc == NULL ||
        read_gold("gold_sensor_init_gyro.bin", DeviceMeasure.gyro, sizeof(DeviceMeasure.gyro)) ||
        read_gold("gold_sensor_init_magmeter.bin", DeviceMeasure.magmeter, sizeof(DeviceMeasure.magmeter)) ||
        read_gold("gold_sensor_init_sts.bin", DeviceMeasure.sts, sizeof(DeviceMeasure.sts)) ||
        read_gold("gold_sensor_init_dss.bin", DeviceMeasure.dss, sizeof(DeviceMeasure.dss)) ||
        read_gold(STATE_GOLD, expected_states, STEP_COUNT * STATE_BYTES) ||
        read_gold(GLOBAL_Y_GOLD, expected_global_y, STEP_COUNT * GLOBAL_Y_BYTES) ||
        read_gold(MAIN_GOLD, expected_main, STEP_COUNT * MAIN_BYTES) ||
        read_gold(IPC_GOLD, expected_ipc, STEP_COUNT * IPC_PAYLOAD_BYTES) ||
        read_gold("gold_cross_flex_dss_altseed_command_ten_thousand_step_final_rwheel.bin", expected_rwheel, sizeof(expected_rwheel)) ||
        read_gold("gold_cross_flex_dss_altseed_command_ten_thousand_step_final_mtq.bin", expected_mtq, sizeof(expected_mtq)) ||
        read_gold("gold_cross_flex_dss_altseed_command_ten_thousand_step_final_thruster.bin", expected_thruster, sizeof(expected_thruster)) ||
        read_gold("gold_cross_flex_dss_altseed_command_ten_thousand_step_final_thruster_lever.bin", expected_thruster_lever, sizeof(expected_thruster_lever)) ||
        read_gold("gold_cross_flex_dss_altseed_command_ten_thousand_step_final_thruster_input.bin", expected_thruster_input, sizeof(expected_thruster_input)) ||
        read_gold("gold_cross_flex_dss_altseed_command_ten_thousand_step_final_thruster_force.bin", expected_thruster_force, sizeof(expected_thruster_force)) ||
        read_gold("gold_cross_flex_dss_altseed_command_ten_thousand_step_final_thruster_torque.bin", expected_thruster_torque, sizeof(expected_thruster_torque))) {
        free(expected_states);
        free(expected_global_y);
        free(expected_main);
        free(expected_ipc);
        return 1;
    }

    dp_device_measure_globals_relocate();
    DeviceMeasure.gps_init_flag = 0;
    dp_device_globals_reset();
    if (seed_dyn_init_devices() != 0) {
        free(expected_states);
        free(expected_global_y);
        free(expected_main);
        free(expected_ipc);
        return 1;
    }
    setup_initial(&initial);
    DynamicInit(&initial);
    DeviceMeasure.dss[0].gaussian_noise_flag = 1u;
    DeviceMeasure.dss[1].gaussian_noise_flag = 0u;
    y[0] = 0.5;
    y[1] = 0.5;
    y[2] = -0.5;
    y[3] = 0.5;
    y[13] = 2.0;
    y[14] = -1.0;
    y[15] = 0.5;
    y[16] = -3.0;
    y[17] = 1.5;
    y[18] = -0.75;
    y[19] = 0.375;
    y[20] = -0.1875;
    y[21] = 0.09375;
    y[22] = -0.046875;
    y[23] = 0.2;
    y[24] = -0.4;
    y[25] = 0.6;
    y[26] = -0.8;
    y[27] = 0.4;
    y[28] = -0.2;
    y[29] = 0.1;
    y[30] = -0.05;
    y[31] = 0.025;
    y[32] = -0.0125;
    RWheel[0].omega = 628.4185307179587;
    RWheel[0].acceleration = 0.0;
    RWheel[0].angular_momentum = 7.981915339118876;
    initial_state.attitude_q.q0 = 0.5;
    initial_state.attitude_q.q1 = 0.5;
    initial_state.attitude_q.q2 = -0.5;
    initial_state.attitude_q.q3 = 0.5;
    initial_state.body_rate.x = 0.001;
    initial_state.body_rate.y = -0.002;
    initial_state.body_rate.z = 0.003;
    initial_state.position_gci.x = 7000000.0;
    initial_state.velocity_gci.y = 7546.0;
    memcpy(core_output, &initial_state, sizeof(initial_state));
    srand(SCENARIO_SEED);

    for (step = 0u; step < STEP_COUNT; ++step) {
        char label[128];

        setup_command(&command, &command_rng);
        dyn_main(&main_output, core_output, &command);
        (void)snprintf(label, sizeof(label), SCENARIO_LABEL "[%u] global y", step + 1u);
        mismatch |= compare_blob(label, y, expected_global_y + step * GLOBAL_Y_BYTES,
                                 GLOBAL_Y_BYTES);
        (void)snprintf(label, sizeof(label),
                       SCENARIO_LABEL "[%u] CoreDynamic first 0x108", step + 1u);
        mismatch |= compare_blob(label, core_output, expected_states + step * STATE_BYTES,
                                 STATE_BYTES);
        (void)snprintf(label, sizeof(label), SCENARIO_LABEL "[%u] main output", step + 1u);
        mismatch |= compare_blob(label, main_output.raw, expected_main + step * MAIN_BYTES,
                                 MAIN_BYTES);
        memset(&actual_ipc, 0, sizeof(actual_ipc));
        dp_send_dyn_tele(&actual_ipc, &main_output, 0u, 0u, 0u, 0u, 0u);
        (void)snprintf(label, sizeof(label), SCENARIO_LABEL "[%u] IPC payload", step + 1u);
        mismatch |= compare_blob(label, actual_ipc.raw + DP_IPC_FLOAT_BASE,
                                 expected_ipc + step * IPC_PAYLOAD_BYTES, IPC_PAYLOAD_BYTES);
    }

    mismatch |= compare_blob(SCENARIO_LABEL " final RWheel", RWheel, expected_rwheel, sizeof(expected_rwheel));
    mismatch |= compare_blob(SCENARIO_LABEL " final MTQ", MTQ, expected_mtq, sizeof(expected_mtq));
    mismatch |= compare_blob(SCENARIO_LABEL " final Thruster scalar/status", &Thruster, expected_thruster,
                             offsetof(DpThrusterRecovered, lever_arm));
    mismatch |= compare_blob(SCENARIO_LABEL " final Thruster lever count", &Thruster.lever_arm.count,
                             expected_thruster + offsetof(DpThrusterRecovered, lever_arm), sizeof(Thruster.lever_arm.count));
    mismatch |= compare_blob(SCENARIO_LABEL " final Thruster input count", &Thruster.force_input.count,
                             expected_thruster + offsetof(DpThrusterRecovered, force_input), sizeof(Thruster.force_input.count));
    mismatch |= compare_blob(SCENARIO_LABEL " final Thruster force count", &Thruster.force_output.count,
                             expected_thruster + offsetof(DpThrusterRecovered, force_output), sizeof(Thruster.force_output.count));
    mismatch |= compare_blob(SCENARIO_LABEL " final Thruster torque count", &Thruster.torque_output.count,
                             expected_thruster + offsetof(DpThrusterRecovered, torque_output), sizeof(Thruster.torque_output.count));
    mismatch |= compare_blob(SCENARIO_LABEL " final Thruster lever", Thruster.lever_arm.data,
                             expected_thruster_lever, sizeof(expected_thruster_lever));
    mismatch |= compare_blob(SCENARIO_LABEL " final Thruster input", Thruster.force_input.data,
                             expected_thruster_input, sizeof(expected_thruster_input));
    mismatch |= compare_blob(SCENARIO_LABEL " final Thruster force", Thruster.force_output.data,
                             expected_thruster_force, sizeof(expected_thruster_force));
    mismatch |= compare_blob(SCENARIO_LABEL " final Thruster torque", Thruster.torque_output.data,
                             expected_thruster_torque, sizeof(expected_thruster_torque));

    free(expected_states);
    free(expected_global_y);
    free(expected_main);
    free(expected_ipc);
    if (mismatch == 0) {
        puts("dyn_main " SCENARIO_LABEL " original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

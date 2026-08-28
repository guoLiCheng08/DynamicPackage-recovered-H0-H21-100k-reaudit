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
#if defined(DP_ORBIT_TIME_BOUNDARY_H22_GLOBAL_Y_THOUSAND_STEP)
#define STATE_GOLD "gold_orbit_time_boundary_h22_global_y_thousand_first_state.bin"
#define MAIN_GOLD "gold_orbit_time_boundary_h22_global_y_thousand_first_out.bin"
#define IPC_GOLD "gold_orbit_time_boundary_h22_global_y_thousand_first_ipc_payload.bin"
#define GLOBAL_Y_GOLD "gold_orbit_time_boundary_h22_global_y_thousand_first_global_y.bin"
#define SCENARIO_LABEL "orbit-time boundary H22 global-y thousand-step"
#define STEP_COUNT 1000u
#elif defined(DP_ORBIT_TIME_BOUNDARY_H22_GLOBAL_Y_HUNDRED_STEP)
#define STATE_GOLD "gold_orbit_time_boundary_h22_global_y_hundred_first_state.bin"
#define MAIN_GOLD "gold_orbit_time_boundary_h22_global_y_hundred_first_out.bin"
#define IPC_GOLD "gold_orbit_time_boundary_h22_global_y_hundred_first_ipc_payload.bin"
#define GLOBAL_Y_GOLD "gold_orbit_time_boundary_h22_global_y_hundred_first_global_y.bin"
#define SCENARIO_LABEL "orbit-time boundary H22 global-y hundred-step"
#define STEP_COUNT 100u
#elif defined(DP_ORBIT_TIME_BOUNDARY_THOUSAND_STEP)
#define STATE_GOLD "gold_orbit_time_boundary_thousand_step_state.bin"
#define MAIN_GOLD "gold_orbit_time_boundary_thousand_step_out.bin"
#define IPC_GOLD "gold_orbit_time_boundary_thousand_step_ipc_payload.bin"
#define SCENARIO_LABEL "orbit-time boundary thousand-step"
#define STEP_COUNT 1000u
#elif defined(DP_ORBIT_TIME_BOUNDARY_HUNDRED_STEP)
#define STATE_GOLD "gold_orbit_time_boundary_hundred_step_state.bin"
#define MAIN_GOLD "gold_orbit_time_boundary_hundred_step_out.bin"
#define IPC_GOLD "gold_orbit_time_boundary_hundred_step_ipc_payload.bin"
#define SCENARIO_LABEL "orbit-time boundary hundred-step"
#define STEP_COUNT 100u
#elif defined(DP_ORBIT_TIME_BOUNDARY)
#define STATE_GOLD "gold_orbit_time_boundary_ten_step_state.bin"
#define MAIN_GOLD "gold_orbit_time_boundary_ten_step_out.bin"
#define IPC_GOLD "gold_orbit_time_boundary_ten_step_ipc_payload.bin"
#define SCENARIO_LABEL "orbit-time boundary ten-step"
#define STEP_COUNT 10u
#elif defined(DP_LEAP_DAY_LEO_THOUSAND_STEP)
#define STATE_GOLD "gold_leap_day_leo_thousand_step_state.bin"
#define MAIN_GOLD "gold_leap_day_leo_thousand_step_out.bin"
#define IPC_GOLD "gold_leap_day_leo_thousand_step_ipc_payload.bin"
#define GLOBAL_Y_GOLD "gold_leap_day_leo_h23_global_y_thousand_first_global_y.bin"
#define SCENARIO_LABEL "leap-day LEO H23 global-y thousand-step"
#define STEP_COUNT 1000u
#elif defined(DP_LEAP_DAY_LEO_HUNDRED_STEP)
#define STATE_GOLD "gold_leap_day_leo_hundred_step_state.bin"
#define MAIN_GOLD "gold_leap_day_leo_hundred_step_out.bin"
#define IPC_GOLD "gold_leap_day_leo_hundred_step_ipc_payload.bin"
#define GLOBAL_Y_GOLD "gold_leap_day_leo_h23_global_y_hundred_first_global_y.bin"
#define SCENARIO_LABEL "leap-day LEO H23 global-y hundred-step"
#define STEP_COUNT 100u
#elif defined(DP_LEAP_DAY_LEO)
#define STATE_GOLD "gold_leap_day_leo_ten_step_state.bin"
#define MAIN_GOLD "gold_leap_day_leo_ten_step_out.bin"
#define IPC_GOLD "gold_leap_day_leo_ten_step_ipc_payload.bin"
#define SCENARIO_LABEL "leap-day LEO ten-step"
#define STEP_COUNT 10u
#elif defined(DP_NONCOL_FLEX_SADA_FLAG1_THOUSAND_STEP)
#define STATE_GOLD "gold_noncollinear_flex_sada_flag1_thousand_step_state.bin"
#define MAIN_GOLD "gold_noncollinear_flex_sada_flag1_thousand_step_out.bin"
#define IPC_GOLD "gold_noncollinear_flex_sada_flag1_thousand_step_ipc_payload.bin"
#define GLOBAL_Y_GOLD "gold_noncollinear_flex_sada_flag1_h24_global_y_thousand_first_global_y.bin"
#define SCENARIO_LABEL "nonzero-flex noncollinear SADA flag1 H24 global-y thousand-step"
#define STEP_COUNT 1000u
#elif defined(DP_NONCOL_FLEX_SADA_FLAG1_HUNDRED_STEP)
#define STATE_GOLD "gold_noncollinear_flex_sada_flag1_hundred_step_state.bin"
#define MAIN_GOLD "gold_noncollinear_flex_sada_flag1_hundred_step_out.bin"
#define IPC_GOLD "gold_noncollinear_flex_sada_flag1_hundred_step_ipc_payload.bin"
#define GLOBAL_Y_GOLD "gold_noncollinear_flex_sada_flag1_h24_global_y_hundred_first_global_y.bin"
#define SCENARIO_LABEL "nonzero-flex noncollinear SADA flag1 H24 global-y hundred-step"
#define STEP_COUNT 100u
#elif defined(DP_NONCOL_FLEX_SADA_FLAG1_TWENTY_STEP)
#define STATE_GOLD "gold_noncollinear_flex_sada_flag1_twenty_step_state.bin"
#define MAIN_GOLD "gold_noncollinear_flex_sada_flag1_twenty_step_out.bin"
#define IPC_GOLD "gold_noncollinear_flex_sada_flag1_twenty_step_ipc_payload.bin"
#define SCENARIO_LABEL "nonzero-flex noncollinear SADA flag1 twenty-step"
#define STEP_COUNT 20u
#elif defined(DP_NONCOL_FLEX_SADA_FLAG1)
#define STATE_GOLD "gold_noncollinear_flex_sada_flag1_ten_step_state.bin"
#define MAIN_GOLD "gold_noncollinear_flex_sada_flag1_ten_step_out.bin"
#define IPC_GOLD "gold_noncollinear_flex_sada_flag1_ten_step_ipc_payload.bin"
#define SCENARIO_LABEL "nonzero-flex noncollinear SADA flag1 ten-step"
#define STEP_COUNT 10u
#else
#define STATE_GOLD "gold_noncollinear_sada_flag1_ten_step_state.bin"
#define MAIN_GOLD "gold_noncollinear_sada_flag1_ten_step_out.bin"
#define IPC_GOLD "gold_noncollinear_sada_flag1_ten_step_ipc_payload.bin"
#define SCENARIO_LABEL "noncollinear SADA flag1 ten-step"
#define STEP_COUNT 10u
#endif
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
#ifdef DP_ORBIT_TIME_BOUNDARY
    initial->initial_time_or_epoch = 86400.0;
    initial->initial_angular_rate_f32[0] = 0.0125f;
    initial->initial_angular_rate_f32[1] = -0.00875f;
    initial->initial_angular_rate_f32[2] = 0.00425f;
    initial->time_parameters[0] = 2031.0;
    initial->time_parameters[1] = 12.0;
    initial->time_parameters[2] = 31.0;
    initial->time_parameters[3] = 23.0;
    initial->time_parameters[4] = 59.0;
    initial->time_parameters[5] = 50.0;
    initial->orbit_elements[0] = 26560000.0;
    initial->orbit_elements[1] = 0.65;
    initial->orbit_elements[2] = 1.1;
    initial->orbit_elements[3] = 1.7;
    initial->orbit_elements[4] = 2.4;
    initial->orbit_elements[5] = 0.9;
#elif defined(DP_LEAP_DAY_LEO)
    initial->initial_time_or_epoch = 43210.0;
    initial->initial_angular_rate_f32[0] = -0.006f;
    initial->initial_angular_rate_f32[1] = 0.011f;
    initial->initial_angular_rate_f32[2] = -0.009f;
    initial->time_parameters[0] = 2024.0;
    initial->time_parameters[1] = 2.0;
    initial->time_parameters[2] = 29.0;
    initial->time_parameters[3] = 12.0;
    initial->time_parameters[4] = 34.0;
    initial->time_parameters[5] = 56.0;
    initial->orbit_elements[0] = 7078137.0;
    initial->orbit_elements[1] = 0.05;
    initial->orbit_elements[2] = 0.9;
    initial->orbit_elements[3] = 1.2;
    initial->orbit_elements[4] = 0.7;
    initial->orbit_elements[5] = 4.2;
#else
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
#endif
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
#ifdef GLOBAL_Y_GOLD
    unsigned char *expected_global_y = NULL;
#endif
    unsigned step;
    int mismatch = 0;

    expected_states = malloc(STEP_COUNT * STATE_BYTES);
    expected_main = malloc(STEP_COUNT * MAIN_BYTES);
    expected_ipc = malloc(STEP_COUNT * IPC_PAYLOAD_BYTES);
#ifdef GLOBAL_Y_GOLD
    expected_global_y = malloc(STEP_COUNT * STATE_BYTES);
#endif
    if (expected_states == NULL || expected_main == NULL || expected_ipc == NULL
#ifdef GLOBAL_Y_GOLD
        || expected_global_y == NULL
#endif
        ||
        read_gold("gold_sensor_init_gyro.bin", DeviceMeasure.gyro, sizeof(DeviceMeasure.gyro)) ||
        read_gold("gold_sensor_init_magmeter.bin", DeviceMeasure.magmeter, sizeof(DeviceMeasure.magmeter)) ||
        read_gold("gold_sensor_init_sts.bin", DeviceMeasure.sts, sizeof(DeviceMeasure.sts)) ||
        read_gold("gold_sensor_init_dss.bin", DeviceMeasure.dss, sizeof(DeviceMeasure.dss)) ||
        read_gold(STATE_GOLD, expected_states, STEP_COUNT * STATE_BYTES) ||
        read_gold(MAIN_GOLD, expected_main, STEP_COUNT * MAIN_BYTES) ||
        read_gold(IPC_GOLD, expected_ipc, STEP_COUNT * IPC_PAYLOAD_BYTES)
#ifdef GLOBAL_Y_GOLD
        || read_gold(GLOBAL_Y_GOLD, expected_global_y, STEP_COUNT * STATE_BYTES)
#endif
        ) {
        free(expected_states);
        free(expected_main);
        free(expected_ipc);
#ifdef GLOBAL_Y_GOLD
        free(expected_global_y);
#endif
        return 1;
    }

    dp_device_measure_globals_relocate();
    DeviceMeasure.gps_init_flag = 0;
    dp_device_globals_reset();
    if (seed_dyn_init_devices() != 0) {
        free(expected_states);
        free(expected_main);
        free(expected_ipc);
#ifdef GLOBAL_Y_GOLD
        free(expected_global_y);
#endif
        return 1;
    }
    setup_initial(&initial);
    DynamicInit(&initial);
    initial_state.attitude_q.q0 = 0.5;
    initial_state.attitude_q.q1 = 0.5;
    initial_state.attitude_q.q2 = -0.5;
    initial_state.attitude_q.q3 = 0.5;
#ifdef DP_ORBIT_TIME_BOUNDARY
    initial_state.body_rate.x = 0.0125;
    initial_state.body_rate.y = -0.00875;
    initial_state.body_rate.z = 0.00425;
    initial_state.position_gci.x = 15000000.0;
    initial_state.position_gci.y = -90000000.0;
    initial_state.position_gci.z = -39000000.0;
    initial_state.velocity_gci.x = -2500.0;
    initial_state.velocity_gci.y = 5500.0;
    initial_state.velocity_gci.z = 1000.0;
#elif defined(DP_LEAP_DAY_LEO)
    initial_state.body_rate.x = -0.006;
    initial_state.body_rate.y = 0.011;
    initial_state.body_rate.z = -0.009;
    initial_state.position_gci.x = 6500000.0;
    initial_state.position_gci.y = -2200000.0;
    initial_state.position_gci.z = 3500000.0;
    initial_state.velocity_gci.x = 2000.0;
    initial_state.velocity_gci.y = 6200.0;
    initial_state.velocity_gci.z = 3000.0;
#else
    initial_state.body_rate.x = 0.001;
    initial_state.body_rate.y = -0.002;
    initial_state.body_rate.z = 0.003;
    initial_state.position_gci.x = 7000000.0;
    initial_state.velocity_gci.y = 7546.0;
#endif
#if defined(DP_NONCOL_FLEX_SADA_FLAG1) || defined(DP_NONCOL_FLEX_SADA_FLAG1_TWENTY_STEP) || \
    defined(DP_NONCOL_FLEX_SADA_FLAG1_HUNDRED_STEP) || defined(DP_NONCOL_FLEX_SADA_FLAG1_THOUSAND_STEP)
    initial_state.flexible_state[0] = 0.0001;
    initial_state.flexible_state[3] = -0.0002;
    initial_state.flexible_state[7] = 0.00005;
    initial_state.flexible_state[10] = 0.001;
    initial_state.flexible_state[14] = -0.0005;
    initial_state.flexible_state[19] = 0.00025;
#endif
#if !defined(DP_ORBIT_TIME_BOUNDARY) && !defined(DP_LEAP_DAY_LEO)
    command.sada_command_flag = 1u;
    command.sada_command_angle[0] = 0.01;
    command.sada_command_angle[1] = -0.01;
    command.inertia_update_flag = 1u;
#endif
    memcpy(core_output, &initial_state, sizeof(initial_state));
#if defined(DP_ORBIT_TIME_BOUNDARY) || defined(DP_LEAP_DAY_LEO)
    srand(12345u);
#else
    srand(1u);
#endif

    for (step = 0u; step < STEP_COUNT; ++step) {
        char label[112];

        dyn_main(&main_output, core_output, &command);
#ifdef DP_ORBIT_TIME_BOUNDARY_DIAGNOSTIC
        if (step == 0u) {
            double dss_sun_input[3];
            double dss_position_input[3];
            FILE *dss_file;
            memcpy(dss_sun_input, core_output + 0x50u, sizeof(dss_sun_input));
            memcpy(dss_position_input, core_output + 0x98u, sizeof(dss_position_input));
            printf("DSS geometry sun_gci=%.17g %.17g %.17g position_gci=%.17g %.17g %.17g\\n",
                   dss_sun_input[0], dss_sun_input[1], dss_sun_input[2],
                   dss_position_input[0], dss_position_input[1], dss_position_input[2]);
            dss_file = fopen("/tmp/orbit_time_boundary_actual_step1_dss.bin", "wb");
            if (dss_file == NULL || fwrite(DeviceMeasure.dss, 1u, sizeof(DeviceMeasure.dss), dss_file) != sizeof(DeviceMeasure.dss) || fclose(dss_file) != 0) {
                free(expected_states);
                free(expected_main);
                free(expected_ipc);
                return 1;
            }
        }
#endif
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
#ifdef GLOBAL_Y_GOLD
        (void)snprintf(label, sizeof(label), SCENARIO_LABEL "[%u] global y", step + 1u);
        mismatch |= compare_blob(label, y, expected_global_y + step * STATE_BYTES, STATE_BYTES);
#endif
    }

    free(expected_states);
    free(expected_main);
    free(expected_ipc);
#ifdef GLOBAL_Y_GOLD
    free(expected_global_y);
#endif
    if (mismatch == 0) {
        puts("dyn_main " SCENARIO_LABEL " original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

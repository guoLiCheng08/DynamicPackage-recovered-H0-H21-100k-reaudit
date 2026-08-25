#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_core_environment.h"
#include "dynamic_recovered.h"
#include "dynamic_time.h"

#define GOLD_DIR "analysis/time_orbit/"

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

static int compare_double_range(const char *label, const double *actual,
                                const double *expected, unsigned count)
{
    unsigned index;
    for (index = 0u; index < count; ++index) {
        uint64_t actual_bits;
        uint64_t expected_bits;
        memcpy(&actual_bits, actual + index, sizeof(actual_bits));
        memcpy(&expected_bits, expected + index, sizeof(expected_bits));
        if (actual_bits != expected_bits) {
            fprintf(stderr, "%s[%u] got=%016llx expected=%016llx\n", label, index,
                    (unsigned long long)actual_bits, (unsigned long long)expected_bits);
            return -1;
        }
    }
    return 0;
}

int main(void)
{
    DpCoreDefaultModel model;
    DpCoreEnvironmentOutputs environment;
    double state[DP_STATE_DIM];
    double expected_state[DP_STATE_DIM];
    uint8_t expected_output[0x148];
    uint8_t torque_raw[0x168];
    uint8_t core_input_raw[0x90];
    DpVec3 total_torque;
    DpVec3 wheel_angular_momentum;
    double time = 0.0;

    if (read_blob("gold_core_pre_rk4_state.bin", state, sizeof(state)) != 0 ||
        read_blob("gold_core_dynamic_torque_step2.bin", torque_raw, sizeof(torque_raw)) != 0 ||
        read_blob("gold_core_dynamic_input_step2.bin", core_input_raw, sizeof(core_input_raw)) != 0 ||
        read_blob("gold_core_dynamic_environment_step2.bin", expected_output,
                  sizeof(expected_output)) != 0 ||
        read_blob("gold_core_dynamic_y_step2.bin", expected_state, sizeof(expected_state)) != 0 ||
        dp_core_default_model_init(&model) != 0) return 1;
    memcpy(&total_torque, torque_raw + 0x128u, sizeof(total_torque));
    memcpy(&wheel_angular_momentum, core_input_raw + 0x18u, sizeof(wheel_angular_momentum));
    total_torque.x = -total_torque.x;
    total_torque.y = -total_torque.y;
    total_torque.z = -total_torque.z;
    TimeInit(2025.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    SpacecraftMass = 1000.0;
    F_I_external.data[0] = 0.0;
    F_I_external.data[1] = 0.0;
    F_I_external.data[2] = 0.0;
    if (dp_core_default_rk4_step_with_terms_and_environment(&model, state, &time, 0.1,
                                                              NULL, NULL, &environment) != 0 ||
        dp_core_default_rk4_step_with_terms_and_environment(
            &model, state, &time, 0.1, &wheel_angular_momentum, &total_torque,
            &environment) != 0 ||
        compare_double_range("state", state, expected_state, DP_STATE_DIM) != 0 ||
        compare_double_range("sun_body", (const double *)&environment.device_environment.sun_body,
                             (const double *)(expected_output + 0x38u), 3u) != 0 ||
        compare_double_range("sun_gci", (const double *)&environment.device_environment.sun_gci,
                             (const double *)(expected_output + 0x50u), 3u) != 0 ||
        compare_double_range("magnetic_body",
                             (const double *)&environment.device_environment.magnetic_body,
                             (const double *)(expected_output + 0x68u), 3u) != 0 ||
        compare_double_range("magnetic_gci", (const double *)&environment.magnetic_gci,
                             (const double *)(expected_output + 0x80u), 3u) != 0 ||
        compare_double_range("calendar", environment.device_environment.time_values,
                             (const double *)(expected_output + 0xc8u), 6u) != 0) return 1;
    puts("CoreDynamic environment bridge original-ELF compare: PASS (bitwise)");
    return 0;
}

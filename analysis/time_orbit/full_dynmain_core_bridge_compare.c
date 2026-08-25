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

static int compare_state(const char *label, const double actual[DP_STATE_DIM],
                         const char *name)
{
    double expected[DP_STATE_DIM];
    unsigned index;
    if (read_blob(name, expected, sizeof(expected)) != 0) return -1;
    for (index = 0u; index < DP_STATE_DIM; ++index) {
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
    double time = 0.0;

    if (read_blob("gold_full_dyn_init_y.bin", state, sizeof(state)) != 0 ||
        dp_core_default_model_init(&model) != 0) return 1;
    TimeInit(2025.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    SpacecraftMass = 1000.0;
    F_I_external.data[0] = 0.0;
    F_I_external.data[1] = 0.0;
    F_I_external.data[2] = 0.0;
    if (dp_core_default_rk4_step_with_terms_and_environment(&model, state, &time, 0.1,
                                                              NULL, NULL, &environment) != 0 ||
        compare_state("full-dyn step1", state, "gold_full_dyn_step1_post_y.bin") != 0 ||
        dp_core_default_rk4_step_with_terms_and_environment(&model, state, &time, 0.1,
                                                              NULL, NULL, &environment) != 0 ||
        compare_state("full-dyn step2", state, "gold_full_dyn_step2_post_y.bin") != 0) return 1;
    puts("dyn_main zero-command CoreDynamic path original-ELF compare: PASS (bitwise)");
    return 0;
}

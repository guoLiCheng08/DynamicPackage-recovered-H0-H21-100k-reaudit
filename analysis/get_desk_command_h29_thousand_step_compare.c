#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_ipc_telemetry.h"
#include "dynamic_recovered.h"
#include "dynamic_satellite_globals.h"

#define GOLD_DIR "analysis/coverage_inventory/p0_command_family/"
#define STEP_COUNT 1000u
#define FLAGS_BYTES 4u
#define INIT_TAIL_BYTES 12u
#define DRC_BYTES 0x70u

static int read_gold(const char *name, void *out, size_t bytes)
{
    char path[512];
    FILE *file;
    size_t count;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    return fclose(file) != 0 || count != bytes ? -1 : 0;
}

static int compare_block(const char *label, const unsigned char *actual,
                         const unsigned char *expected, size_t bytes)
{
    size_t index;
    unsigned mismatches = 0u;

    for (index = 0u; index < bytes; ++index) {
        if (actual[index] != expected[index]) {
            if (mismatches < 8u) {
                (void)printf("%s byte+0x%zx actual=%02x expected=%02x\n", label, index,
                             (unsigned)actual[index], (unsigned)expected[index]);
            }
            ++mismatches;
        }
    }
    (void)printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

static int seed_step(unsigned step)
{
    unsigned index;
    int failed = 0;

    failed |= dp_ipc_seed_float_for_test(64u, (float)step + 0.25f) != 0;
    failed |= dp_ipc_seed_float_for_test(65u, -(float)step - 0.5f) != 0;
    failed |= dp_ipc_seed_float_for_test(66u, (float)step * 0.125f + 1.0f) != 0;
    for (index = 0u; index < 17u; ++index) {
        failed |= dp_ipc_seed_u8_for_test(20u + index,
                                           (unsigned char)(step * 17u + index + 0x80u)) != 0;
    }
    return failed;
}

int main(void)
{
    unsigned char *expected_flags = NULL;
    unsigned char *expected_init_tail = NULL;
    unsigned char *expected_drc = NULL;
    unsigned step;
    int mismatch = 0;

    expected_flags = malloc(STEP_COUNT * FLAGS_BYTES);
    expected_init_tail = malloc(STEP_COUNT * INIT_TAIL_BYTES);
    expected_drc = malloc(STEP_COUNT * DRC_BYTES);
    if (expected_flags == NULL || expected_init_tail == NULL || expected_drc == NULL ||
        read_gold("gold_get_desk_command_h29_thousand_first_flags.bin", expected_flags,
                  STEP_COUNT * FLAGS_BYTES) != 0 ||
        read_gold("gold_get_desk_command_h29_thousand_first_init_tail.bin", expected_init_tail,
                  STEP_COUNT * INIT_TAIL_BYTES) != 0 ||
        read_gold("gold_get_desk_command_h29_thousand_first_drc.bin", expected_drc,
                  STEP_COUNT * DRC_BYTES) != 0) {
        free(expected_flags);
        free(expected_init_tail);
        free(expected_drc);
        return 1;
    }

    DynamicDllInit();
    dp_desk_command_state_reset_for_test();
    for (step = 0u; step < STEP_COUNT; ++step) {
        const DpInitialConditions *initial;
        char label[112];

        if (seed_step(step) != 0) {
            (void)fputs("cannot seed H29 IPC input\n", stderr);
            mismatch = 1;
            break;
        }
        getDeskCommand();
        initial = dp_dynamic_dll_initial_conditions();
        (void)snprintf(label, sizeof(label), "H29 getDeskCommand[%u] flags", step + 1u);
        mismatch |= compare_block(label, dp_desk_command_flags_for_test(),
                                  expected_flags + step * FLAGS_BYTES, FLAGS_BYTES);
        (void)snprintf(label, sizeof(label), "H29 getDeskCommand[%u] init tail", step + 1u);
        mismatch |= compare_block(label, (const unsigned char *)initial->initial_angular_rate_f32,
                                  expected_init_tail + step * INIT_TAIL_BYTES, INIT_TAIL_BYTES);
        (void)snprintf(label, sizeof(label), "H29 getDeskCommand[%u] DRC", step + 1u);
        mismatch |= compare_block(label, dp_desk_command_drc_for_test(),
                                  expected_drc + step * DRC_BYTES, DRC_BYTES);
    }
    close_shared();
    free(expected_flags);
    free(expected_init_tail);
    free(expected_drc);
    if (mismatch == 0) {
        (void)puts("PASS: H29 getDeskCommand thousand-step bitwise compare");
        return 0;
    }
    return 1;
}

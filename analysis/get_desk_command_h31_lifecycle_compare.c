#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "dynamic_ipc_telemetry.h"
#include "dynamic_recovered.h"
#include "dynamic_satellite_globals.h"

#define GOLD_DIR "analysis/coverage_inventory/p0_command_family/"
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

static int compare_blob(const char *label, const void *actual, const void *expected, size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;
    unsigned mismatches = 0u;

    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            if (mismatches < 8u) {
                (void)printf("%s byte+0x%zx actual=%02x expected=%02x\n", label, index,
                             (unsigned)a[index], (unsigned)e[index]);
            }
            ++mismatches;
        }
    }
    (void)printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

static int seed_h31_input(void)
{
    unsigned index;
    int failed = 0;

    failed |= dp_ipc_seed_float_for_test(64u, 6.25f) != 0;
    failed |= dp_ipc_seed_float_for_test(65u, -7.5f) != 0;
    failed |= dp_ipc_seed_float_for_test(66u, 8.75f) != 0;
    for (index = 0u; index < 17u; ++index) {
        failed |= dp_ipc_seed_u8_for_test(20u + index, (unsigned char)(0xa0u + index)) != 0;
    }
    return failed;
}

static int compare_phase(const char *phase, const unsigned char *flags, const float *tail,
                         const unsigned char *drc, const unsigned char *gold_flags,
                         const unsigned char *gold_tail, const unsigned char *gold_drc)
{
    int mismatch = 0;
    char label[96];

    (void)snprintf(label, sizeof(label), "H31 %s flags", phase);
    mismatch |= compare_blob(label, flags, gold_flags, FLAGS_BYTES);
    (void)snprintf(label, sizeof(label), "H31 %s init angular-rate tail", phase);
    mismatch |= compare_blob(label, tail, gold_tail, INIT_TAIL_BYTES);
    (void)snprintf(label, sizeof(label), "H31 %s DRC", phase);
    mismatch |= compare_blob(label, drc, gold_drc, DRC_BYTES);
    return mismatch;
}

int main(void)
{
    unsigned char gold_open_flags[FLAGS_BYTES];
    unsigned char gold_open_tail[INIT_TAIL_BYTES];
    unsigned char gold_open_drc[DRC_BYTES];
    unsigned char gold_reopen_flags[FLAGS_BYTES];
    unsigned char gold_reopen_tail[INIT_TAIL_BYTES];
    unsigned char gold_reopen_drc[DRC_BYTES];
    const DpInitialConditions *initial;
    int mismatch = 0;

    close_shared();
    (void)shm_unlink(DP_IPC_SHM_NAME);
    if (read_gold("gold_get_desk_command_h31_lifecycle_first_after_open_flags.bin", gold_open_flags, sizeof(gold_open_flags)) != 0 ||
        read_gold("gold_get_desk_command_h31_lifecycle_first_after_open_init_tail.bin", gold_open_tail, sizeof(gold_open_tail)) != 0 ||
        read_gold("gold_get_desk_command_h31_lifecycle_first_after_open_drc.bin", gold_open_drc, sizeof(gold_open_drc)) != 0 ||
        read_gold("gold_get_desk_command_h31_lifecycle_first_after_reopen_flags.bin", gold_reopen_flags, sizeof(gold_reopen_flags)) != 0 ||
        read_gold("gold_get_desk_command_h31_lifecycle_first_after_reopen_init_tail.bin", gold_reopen_tail, sizeof(gold_reopen_tail)) != 0 ||
        read_gold("gold_get_desk_command_h31_lifecycle_first_after_reopen_drc.bin", gold_reopen_drc, sizeof(gold_reopen_drc)) != 0) {
        return 1;
    }

    DynamicDllInit();
    dp_desk_command_state_reset_for_test();
    if (init_shared() != 0 || seed_h31_input() != 0) {
        (void)fputs("H31 cannot create/seed controlled shared IPC frame\n", stderr);
        close_shared();
        (void)shm_unlink(DP_IPC_SHM_NAME);
        return 1;
    }
    getDeskCommand();
    initial = dp_dynamic_dll_initial_conditions();
    mismatch |= compare_phase("after-open", dp_desk_command_flags_for_test(),
                              initial->initial_angular_rate_f32, dp_desk_command_drc_for_test(),
                              gold_open_flags, gold_open_tail, gold_open_drc);

    close_shared();
    if (init_shared() != 0) {
        (void)fputs("H31 cannot reopen controlled shared IPC frame\n", stderr);
        (void)shm_unlink(DP_IPC_SHM_NAME);
        return 1;
    }
    getDeskCommand();
    mismatch |= compare_phase("after-reopen", dp_desk_command_flags_for_test(),
                              initial->initial_angular_rate_f32, dp_desk_command_drc_for_test(),
                              gold_reopen_flags, gold_reopen_tail, gold_reopen_drc);

    close_shared();
    (void)shm_unlink(DP_IPC_SHM_NAME);
    if (mismatch == 0) {
        (void)puts("PASS: H31 getDeskCommand IPC lifecycle bitwise compare");
        return 0;
    }
    return 1;
}

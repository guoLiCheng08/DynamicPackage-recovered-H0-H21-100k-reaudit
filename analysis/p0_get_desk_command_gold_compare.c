/* P0 getDeskCommand：受控共享输入的原 ELF 位级比较。 */
#include "dynamic_ipc_telemetry.h"
#include "dynamic_recovered.h"
#include "dynamic_satellite_globals.h"

#include <stdio.h>
#include <string.h>

#define GOLD_DIR "analysis/coverage_inventory/p0_command_family/"

static int read_exact(const char *name, unsigned char *dst, size_t size)
{
    char path[256];
    FILE *fp;
    int ok = 0;

    if (snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name) < 0) {
        return 0;
    }
    fp = fopen(path, "rb");
    if (fp != NULL) {
        ok = fread(dst, 1, size, fp) == size && fgetc(fp) == EOF;
        (void)fclose(fp);
    }
    return ok;
}

static int compare_block(const char *label, const char *gold_name,
                         const void *actual, size_t size)
{
    unsigned char expected[112];
    size_t i;
    size_t differences = 0;

    if (size > sizeof(expected) || !read_exact(gold_name, expected, size)) {
        (void)fprintf(stderr, "cannot read %s%s\n", GOLD_DIR, gold_name);
        return 2;
    }
    for (i = 0; i < size; ++i) {
        differences += expected[i] != ((const unsigned char *)actual)[i];
    }
    (void)printf("P0 getDeskCommand %s differences: %zu/%zu\n", label, differences, size);
    return differences == 0 ? 0 : 1;
}

static int seed_inputs(void)
{
    int failed = 0;
    const unsigned char values[17] = {
        0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91
    };
    size_t i;

    failed |= dp_ipc_seed_float_for_test(64u, 1.25f) != 0;
    failed |= dp_ipc_seed_float_for_test(65u, -2.5f) != 0;
    failed |= dp_ipc_seed_float_for_test(66u, 3.75f) != 0;
    for (i = 0; i < sizeof(values); ++i) {
        failed |= dp_ipc_seed_u8_for_test((uint32_t)(20u + i), values[i]) != 0;
    }
    return failed;
}

int main(void)
{
    unsigned char init_tail[12];
    const DpInitialConditions *initial;
    int failed = 0;

    DynamicDllInit();
    dp_desk_command_state_reset_for_test();
    if (seed_inputs() != 0) {
        (void)fprintf(stderr, "cannot seed controlled IPC input\n");
        return 2;
    }
    getDeskCommand();

    initial = dp_dynamic_dll_initial_conditions();
    memcpy(init_tail, initial->initial_angular_rate_f32, sizeof(init_tail));
    failed |= compare_block("flags", "gold_get_desk_command_p0_flags.bin",
                            dp_desk_command_flags_for_test(), 4u);
    failed |= compare_block("init float tail", "gold_get_desk_command_p0_init_float_tail.bin",
                            init_tail, sizeof(init_tail));
    failed |= compare_block("DRC", "gold_get_desk_command_p0_drc.bin",
                            dp_desk_command_drc_for_test(), 0x70u);
    close_shared();
    (void)printf("P0 getDeskCommand original-ELF compare: %s (bitwise)\n",
                 failed == 0 ? "PASS" : "FAIL");
    return failed == 0 ? 0 : 1;
}

/* DynamicDllInit P0：共享输入不可读的原 ELF 默认启动分支比较。 */
#include "dynamic_devices.h"
#include "dynamic_satellite_globals.h"

#include <stdio.h>
#include <string.h>

#define GOLD_DIR "analysis/coverage_inventory/target_closure/"

static int read_exact(const char *path, void *dst, size_t size)
{
    FILE *fp = fopen(path, "rb");
    int ok = 0;

    if (fp != NULL) {
        ok = fread(dst, 1, size, fp) == size && fgetc(fp) == EOF;
        (void)fclose(fp);
    }
    return ok;
}

static size_t mismatch_count(const unsigned char *left,
                             const unsigned char *right,
                             size_t size)
{
    size_t count = 0;
    size_t i;

    for (i = 0; i < size; ++i) {
        count += left[i] != right[i];
    }
    return count;
}

static int compare_block(const char *name, const char *gold_name,
                         const void *actual, size_t size)
{
    unsigned char expected[232];
    char path[256];
    size_t mismatches;

    if (snprintf(path, sizeof(path), "%s%s", GOLD_DIR, gold_name) < 0 ||
        size > sizeof(expected) || !read_exact(path, expected, size)) {
        (void)fprintf(stderr, "cannot read %s\n", path);
        return 2;
    }
    mismatches = mismatch_count(expected, actual, size);
    (void)printf("DynamicDllInit P0 %s mismatched bytes: %zu/%zu\n",
                 name, mismatches, size);
    return mismatches == 0 ? 0 : 1;
}

int main(void)
{
    int failed = 0;

    DynamicDllInit();

    failed |= compare_block("initial conditions", "gold_dynamic_dll_init_p0_init.bin",
                            dp_dynamic_dll_initial_conditions(),
                            sizeof(DpInitialConditions));
    failed |= compare_block("SADA", "gold_dynamic_dll_init_p0_sada.bin",
                            &SADA, sizeof(SADA));
    failed |= compare_block("WheelGroup angular-momentum backing",
                            "gold_dynamic_dll_init_p0_wheelgroup_h.bin",
                            WheelGroup.angular_momentum.data,
                            3u * sizeof(double));
    failed |= compare_block("WheelGroup torque backing",
                            "gold_dynamic_dll_init_p0_wheelgroup_t.bin",
                            WheelGroup.torque.data,
                            3u * sizeof(double));
    failed |= compare_block("MTQ group-moment backing",
                            "gold_dynamic_dll_init_p0_mtqgroup_moment.bin",
                            MTQ_Group.group_moment.data,
                            3u * sizeof(double));
    failed |= compare_block("Thruster force backing",
                            "gold_dynamic_dll_init_p0_thruster_force.bin",
                            Thruster.force_output.data,
                            3u * sizeof(double));
    failed |= compare_block("Thruster torque backing",
                            "gold_dynamic_dll_init_p0_thruster_torque.bin",
                            Thruster.torque_output.data,
                            3u * sizeof(double));

    (void)printf("DynamicDllInit P0 original-ELF compare: %s (bitwise, default startup branch)\n",
                 failed == 0 ? "PASS" : "FAIL");
    return failed == 0 ? 0 : 1;
}

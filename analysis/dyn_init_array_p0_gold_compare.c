/* dyn_init_array P0：30-double 非零输入的原 ELF 位级比较。 */
#include "dynamic_devices.h"
#include "dynamic_satellite_globals.h"

#include <stdio.h>
#include <string.h>

#define GOLD_DIR "analysis/coverage_inventory/p0_dyn_init_array/"

static int read_exact(const char *name, void *dst, size_t size)
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
    unsigned char expected[240];
    size_t i;
    size_t differences = 0;

    if (size > sizeof(expected) || !read_exact(gold_name, expected, size)) {
        (void)fprintf(stderr, "cannot read %s%s\n", GOLD_DIR, gold_name);
        return 2;
    }
    for (i = 0; i < size; ++i) {
        differences += expected[i] != ((const unsigned char *)actual)[i];
    }
    (void)printf("dyn_init_array P0 %s mismatched bytes: %zu/%zu\n",
                 label, differences, size);
    return differences == 0 ? 0 : 1;
}

int main(void)
{
    double input[30];
    size_t i;
    int failed = 0;

    for (i = 0; i < 30; ++i) {
        input[i] = (double)(i + 1) * 1.125;
    }
    dyn_init_array(input);

    failed |= compare_block("caller input", "gold_dyn_init_array_p0_input_after.bin",
                            input, sizeof(input));
    failed |= compare_block("WheelGroup angular-momentum backing",
                            "gold_dyn_init_array_p0_wheel_h.bin",
                            WheelGroup.angular_momentum.data, 3u * sizeof(double));
    failed |= compare_block("WheelGroup torque backing",
                            "gold_dyn_init_array_p0_wheel_t.bin",
                            WheelGroup.torque.data, 3u * sizeof(double));
    failed |= compare_block("MTQ group-moment backing", "gold_dyn_init_array_p0_mtq.bin",
                            MTQ_Group.group_moment.data, 3u * sizeof(double));
    failed |= compare_block("Thruster force backing", "gold_dyn_init_array_p0_thr_force.bin",
                            Thruster.force_output.data, 3u * sizeof(double));
    failed |= compare_block("Thruster torque backing", "gold_dyn_init_array_p0_thr_torque.bin",
                            Thruster.torque_output.data, 3u * sizeof(double));
    (void)printf("dyn_init_array P0 original-ELF compare: %s (bitwise)\n",
                 failed == 0 ? "PASS" : "FAIL");
    return failed == 0 ? 0 : 1;
}

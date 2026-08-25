/* UpdateCoreDynInput P4：原 ELF 隔离金标的完整 0x90 输出帧位级比较。 */
#include "dynamic_core_layout.h"
#include "dynamic_devices.h"
#include "dynamic_flex.h"
#include "dynamic_satellite_globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static size_t mismatched_bytes(const unsigned char *left,
                               const unsigned char *right,
                               size_t size)
{
    size_t mismatches = 0;
    size_t i;

    for (i = 0; i < size; ++i) {
        mismatches += left[i] != right[i];
    }
    return mismatches;
}

int main(void)
{
    unsigned char expected[sizeof(DpCoreDynInput)];
    unsigned char actual[sizeof(DpCoreDynInput)];
    const char *gold =
        "analysis/coverage_inventory/target_closure/"
        "gold_update_core_dyn_input_p4_output.bin";
    size_t mismatch_count;

    if (!read_exact(gold, expected, sizeof(expected))) {
        (void)fprintf(stderr, "cannot read %s\n", gold);
        return 2;
    }

    dp_device_globals_reset();
    WheelGroup.angular_momentum.data[0] = 1.125;
    WheelGroup.angular_momentum.data[1] = -2.25;
    WheelGroup.angular_momentum.data[2] = 3.5;
    WheelGroup.torque.data[0] = -4.75;
    WheelGroup.torque.data[1] = 5.875;
    WheelGroup.torque.data[2] = -6.0;
    MTQ_Group.group_moment.data[0] = 7.25;
    MTQ_Group.group_moment.data[1] = -8.5;
    MTQ_Group.group_moment.data[2] = 9.75;
    Thruster.force_output.data[0] = -10.125;
    Thruster.force_output.data[1] = 11.25;
    Thruster.force_output.data[2] = -12.5;
    Thruster.torque_output.data[0] = 13.625;
    Thruster.torque_output.data[1] = -14.75;
    Thruster.torque_output.data[2] = 15.875;
    SADA.current_angle[0] = 16.125;
    SADA.current_angle[1] = -17.25;

    (void)memset(actual, 0xa5, sizeof(actual));
    UpdateCoreDynInput(actual);

    mismatch_count = mismatched_bytes(actual, expected, sizeof(actual));
    (void)printf("UpdateCoreDynInput P4 original-ELF compare: %s (bitwise; %zu/%zu bytes differ)\n",
                 mismatch_count == 0 ? "PASS" : "FAIL",
                 mismatch_count, sizeof(actual));
    return mismatch_count == 0 ? 0 : 1;
}

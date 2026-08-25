#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_devices.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_gold(const char *name, void *out, size_t bytes)
{
    char path[256];
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
    size_t i;
    unsigned mismatches = 0u;

    for (i = 0u; i < bytes; ++i) {
        if (a[i] != e[i]) {
            if (mismatches < 8u) {
                printf("%s byte+0x%zx actual=%02x expected=%02x\n", label, i,
                       (unsigned)a[i], (unsigned)e[i]);
            }
            ++mismatches;
        }
    }
    printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

static void init_wheel(DpReactionWheelRecovered *wheel, double omega, double acceleration)
{
    wheel->add_gaussian_noise_flag = 0u;
    wheel->omega_limit = 1.0;
    wheel->torque_limit = 0.05;
    wheel->inertia = 0.1;
    wheel->omega = omega;
    wheel->acceleration = acceleration;
}

int main(void)
{
    DpReactionWheelRecovered wheels[DP_WHEEL_COUNT];
    unsigned char expected[sizeof(wheels[0])];
    double command[DP_WHEEL_COUNT] = {0.0, 0.0, 0.0, 0.0};
    int mismatch = 0;

    if (read_gold("../golden/dyn_main_global_abi/pre_rwheel_4.bin", wheels, sizeof(wheels)) != 0) return 1;
    init_wheel(&wheels[0], 1.0, 0.25);
    command[0] = 0.05;
    if (read_gold("gold_wheel_exact_limit_pos.bin", expected, sizeof(expected)) != 0) return 1;
    dp_set_wheel_acc(wheels, command, NULL, NULL);
    mismatch |= compare_blob("wheel exact positive limit", &wheels[0], expected, sizeof(expected));

    if (read_gold("../golden/dyn_main_global_abi/pre_rwheel_4.bin", wheels, sizeof(wheels)) != 0) return 1;
    init_wheel(&wheels[0], -2.0, 0.0);
    command[0] = -1.0;
    if (read_gold("gold_wheel_negative_high_torque_limit.bin", expected, sizeof(expected)) != 0) return 1;
    dp_set_wheel_acc(wheels, command, NULL, NULL);
    mismatch |= compare_blob("wheel negative high speed torque limit", &wheels[0], expected, sizeof(expected));

    if (read_gold("../golden/dyn_main_global_abi/pre_rwheel_4.bin", wheels, sizeof(wheels)) != 0) return 1;
    init_wheel(&wheels[0], 2.0, 0.0);
    command[0] = 1.0;
    if (read_gold("gold_wheel_positive_high_torque_limit.bin", expected, sizeof(expected)) != 0) return 1;
    dp_set_wheel_acc(wheels, command, NULL, NULL);
    mismatch |= compare_blob("wheel positive high speed torque limit", &wheels[0], expected, sizeof(expected));

    if (mismatch == 0) puts("wheel exact-boundary original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

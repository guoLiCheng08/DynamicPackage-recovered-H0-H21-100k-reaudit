#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_devices.h"
#include "dynamic_satellite_globals.h"

#define GOLD_DIR "analysis/time_orbit/"

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

static int compare_blob(const char *label, const void *actual, const void *expected,
                        size_t bytes)
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

int main(void)
{
    const double initial[30] = {
        0.01, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        2024.0, 10.0, 28.0, 4.0, 16.0, 16.0,
        6910593.0, 0.00058, 1.7018327148814874, 0.24732846154513649,
        3.0651957525056472, 6.2273473025797976,
        120.0, -0.63, 1.3, -0.63, 361.0, -0.5, 1.3, -0.5, 469.0, 600.0
    };
    unsigned char expected_rwheel[sizeof(RWheel)];
    unsigned char expected_mtq[sizeof(MTQ)];
    unsigned char expected_sada[sizeof(SADA)];
    int mismatch;
    double command[16] = {0.0};
    double output_words[0x1e8u / sizeof(double)];
    double state_words[0x200u / sizeof(double)];

    if (read_gold("gold_dyn_main_array_h28_post_rwheel.bin", expected_rwheel,
                  sizeof(expected_rwheel)) != 0 ||
        read_gold("gold_dyn_main_array_h28_post_mtq.bin", expected_mtq,
                  sizeof(expected_mtq)) != 0 ||
        read_gold("gold_dyn_main_array_h28_post_sada.bin", expected_sada,
                  sizeof(expected_sada)) != 0) {
        return 1;
    }
    dyn_init_array(initial);
    command[0] = 0.005;
    command[2] = -0.003;
    command[4] = 0.008;
    command[7] = -0.006;
    command[10] = 1.0;
    command[11] = 0.01;
    command[12] = -0.005;
    command[14] = 1.0;
    command[15] = 1.0;
    memset(output_words, 0xa5, sizeof(output_words));
    memset(state_words, 0x5a, sizeof(state_words));
    dyn_main_array(output_words, state_words, command);
    mismatch = compare_blob("H28 dyn_main_array post RWheel", RWheel, expected_rwheel,
                            sizeof(expected_rwheel));
    mismatch |= compare_blob("H28 dyn_main_array post MTQ", MTQ, expected_mtq,
                             sizeof(expected_mtq));
    mismatch |= compare_blob("H28 dyn_main_array post SADA", &SADA, expected_sada,
                             sizeof(expected_sada));
    return mismatch == 0 ? 0 : 1;
}

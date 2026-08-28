#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_recovered.h"
#include "dynamic_satellite_globals.h"

#define GOLD_DIR "analysis/time_orbit/"
#define STEP_COUNT 1000u
#define LEGACY_OUT_BYTES 0x1e8u
#define LEGACY_STATE_BYTES 0x200u
#define OUT_GOLD "gold_dyn_main_array_h28_thousand_first_out.bin"
#define STATE_GOLD "gold_dyn_main_array_h28_thousand_first_state.bin"

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
    double command[16] = {0.0};
    double output_words[LEGACY_OUT_BYTES / sizeof(double)];
    double state_words[LEGACY_STATE_BYTES / sizeof(double)];
    unsigned char *expected_out = NULL;
    unsigned char *expected_state = NULL;
    unsigned step;
    int mismatch = 0;

    command[0] = 0.005;
    command[2] = -0.003;
    command[4] = 0.008;
    command[7] = -0.006;
    command[10] = 1.0;
    command[11] = 0.01;
    command[12] = -0.005;
    command[14] = 1.0;
    command[15] = 1.0;

    expected_out = malloc(STEP_COUNT * LEGACY_OUT_BYTES);
    expected_state = malloc(STEP_COUNT * LEGACY_STATE_BYTES);
    if (expected_out == NULL || expected_state == NULL ||
        read_gold(OUT_GOLD, expected_out, STEP_COUNT * LEGACY_OUT_BYTES) != 0 ||
        read_gold(STATE_GOLD, expected_state, STEP_COUNT * LEGACY_STATE_BYTES) != 0) {
        free(expected_out);
        free(expected_state);
        return 1;
    }

    dyn_init_array(initial);
    for (step = 0u; step < STEP_COUNT; ++step) {
        char label[112];

        memset(output_words, 0xa5, sizeof(output_words));
        memset(state_words, 0x5a, sizeof(state_words));
        dyn_main_array(output_words, state_words, command);
        (void)snprintf(label, sizeof(label), "H28 dyn_main_array[%u] legacy out", step + 1u);
        mismatch |= compare_blob(label, output_words,
                                 expected_out + step * LEGACY_OUT_BYTES,
                                 LEGACY_OUT_BYTES);
        (void)snprintf(label, sizeof(label), "H28 dyn_main_array[%u] legacy state", step + 1u);
        mismatch |= compare_blob(label, state_words,
                                 expected_state + step * LEGACY_STATE_BYTES,
                                 LEGACY_STATE_BYTES);
    }

    free(expected_out);
    free(expected_state);
    if (mismatch == 0) {
        (void)puts("PASS: H28 dyn_main_array thousand-step bitwise compare");
        return 0;
    }
    return 1;
}

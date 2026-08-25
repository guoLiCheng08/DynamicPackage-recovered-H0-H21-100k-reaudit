#include <stdio.h>
#include <string.h>

#include "dynamic_recovered.h"
#include "dynamic_satellite_globals.h"

#define DP_ARRAY_OUT_BYTES 0x1e8u
#define DP_ARRAY_STATE_BYTES 0x200u

static int load_gold(const char *path, unsigned char *destination, size_t bytes)
{
    FILE *file = fopen(path, "rb");
    size_t actual;

    if (file == NULL) {
        (void)fprintf(stderr, "cannot open gold: %s\n", path);
        return 1;
    }
    actual = fread(destination, 1u, bytes, file);
    if (fclose(file) != 0 || actual != bytes) {
        (void)fprintf(stderr, "invalid gold length: %s\n", path);
        return 1;
    }
    return 0;
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
    unsigned char output[DP_ARRAY_OUT_BYTES];
    unsigned char state[DP_ARRAY_STATE_BYTES];
    unsigned char expected_output[DP_ARRAY_OUT_BYTES];
    unsigned char expected_state[DP_ARRAY_STATE_BYTES];

    if (load_gold("analysis/time_orbit/dyn_main_array_gold_out.bin", expected_output,
                  sizeof(expected_output)) != 0 ||
        load_gold("analysis/time_orbit/dyn_main_array_gold_state.bin", expected_state,
                  sizeof(expected_state)) != 0) {
        return 1;
    }
    dyn_init_array(initial);
    memset(output, 0xa5, sizeof(output));
    memset(state, 0x5a, sizeof(state));
    dyn_main_array((double *)output, (double *)state, command);
    if (memcmp(output, expected_output, sizeof(output)) != 0 ||
        memcmp(state, expected_state, sizeof(state)) != 0) {
        size_t index;

        for (index = 0u; index < sizeof(output); ++index) {
            if (output[index] != expected_output[index]) {
                (void)printf("legacy_out first mismatch +%#zx: got=%#x expected=%#x\\n", index,
                             (unsigned)output[index], (unsigned)expected_output[index]);
                break;
            }
        }
        for (index = 0u; index < sizeof(state); ++index) {
            if (state[index] != expected_state[index]) {
                (void)printf("legacy_state first mismatch +%#zx: got=%#x expected=%#x\\n", index,
                             (unsigned)state[index], (unsigned)expected_state[index]);
                break;
            }
        }
        (void)puts("dyn_main_array complete legacy-buffer output mismatch");
        return 1;
    }
    (void)puts("PASS: dyn_main_array controlled original-ELF gold compare");
    return 0;
}

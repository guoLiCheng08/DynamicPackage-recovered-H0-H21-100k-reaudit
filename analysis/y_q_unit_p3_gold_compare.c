#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

#define NORMAL_GOLD "analysis/time_orbit/gold_y_q_unit_p3_state.bin"
#define SMALL_GOLD "analysis/time_orbit/gold_y_q_unit_smallnorm_p3_state.bin"
#define STDOUT_GOLD "analysis/time_orbit/gold_y_q_unit_p3_stdout.bin"
#define STDOUT_ACTUAL "build/y_q_unit_p3_stdout.actual"

static int read_exact(const char *path, unsigned char *destination, size_t length)
{
    FILE *file = fopen(path, "rb");
    size_t count;

    if (file == NULL) {
        perror(path);
        return 0;
    }
    count = fread(destination, 1u, length, file);
    if (fclose(file) != 0 || count != length) {
        fprintf(stderr, "%s: expected %zu bytes, received %zu\n", path, length, count);
        return 0;
    }
    return 1;
}

static int compare_blob(const char *name, const unsigned char *actual,
                        const unsigned char *expected, size_t length)
{
    size_t index;

    if (memcmp(actual, expected, length) == 0) {
        return 1;
    }
    for (index = 0u; index < length; ++index) {
        if (actual[index] != expected[index]) {
            fprintf(stderr, "%s mismatch +0x%zx actual=%02x expected=%02x\n", name,
                    index, actual[index], expected[index]);
            break;
        }
    }
    return 0;
}

static void initialize_state(double state[DP_STATE_DIM], double base,
                             double q0, double q1, double q2, double q3)
{
    size_t index;

    for (index = 0u; index < DP_STATE_DIM; ++index) {
        state[index] = base + (double)index;
    }
    state[0] = q0;
    state[1] = q1;
    state[2] = q2;
    state[3] = q3;
}

int main(void)
{
    double state[DP_STATE_DIM];
    unsigned char expected_state[sizeof state];
    unsigned char expected_stdout[71];
    unsigned char actual_stdout[sizeof expected_stdout];

    initialize_state(state, 1000.0, 0.5, -1.5, 2.5, -3.5);
    if (!read_exact(NORMAL_GOLD, expected_state, sizeof expected_state)) {
        return 1;
    }
    y_q_unit(state);
    if (!compare_blob("normal y_q_unit state", (const unsigned char *)state,
                      expected_state, sizeof state)) {
        return 1;
    }

    initialize_state(state, 2000.0, 0.001, -0.002, 0.003, -0.004);
    if (!read_exact(SMALL_GOLD, expected_state, sizeof expected_state) ||
        !read_exact(STDOUT_GOLD, expected_stdout, sizeof expected_stdout)) {
        return 1;
    }
    if (freopen(STDOUT_ACTUAL, "wb", stdout) == NULL) {
        perror(STDOUT_ACTUAL);
        return 1;
    }
    y_q_unit(state);
    if (fflush(stdout) != 0 || fclose(stdout) != 0) {
        perror(STDOUT_ACTUAL);
        return 1;
    }
    if (!read_exact(STDOUT_ACTUAL, actual_stdout, sizeof actual_stdout) ||
        !compare_blob("small-norm y_q_unit state", (const unsigned char *)state,
                      expected_state, sizeof state) ||
        !compare_blob("small-norm y_q_unit stdout", actual_stdout, expected_stdout,
                      sizeof actual_stdout)) {
        return 1;
    }

    fputs("y_q_unit P3 original-ELF compare: PASS (bitwise)\n", stderr);
    return 0;
}

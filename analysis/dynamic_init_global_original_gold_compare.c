#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"
#include "dynamic_time.h"

#ifndef GOLD
#define GOLD "analysis/golden/dynamic_init_global_abi/"
#endif

_Static_assert(sizeof(DpInitialConditions) == 0xe8u,
               "DynamicInit input ABI must remain 0xe8 bytes");

static int read_exact(const char *name, void *dst, size_t bytes)
{
    char path[512];
    FILE *file;
    size_t got;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return -1;
    }
    got = fread(dst, 1u, bytes, file);
    if (fclose(file) != 0 || got != bytes) {
        return -1;
    }
    return 0;
}

static int compare_blob(const char *label, const void *actual, const void *expected,
                        size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;
    unsigned count = 0u;

    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            if (count < 6u) {
                const size_t slot = index / sizeof(double);
                uint64_t actual_bits = 0u;
                uint64_t expected_bits = 0u;
                memcpy(&actual_bits, a + slot * sizeof(actual_bits), sizeof(actual_bits));
                memcpy(&expected_bits, e + slot * sizeof(expected_bits), sizeof(expected_bits));
                printf("%s byte+0x%zx slot=%zu actual=%016llx expected=%016llx\n",
                       label, index, slot, (unsigned long long)actual_bits,
                       (unsigned long long)expected_bits);
            }
            ++count;
        }
    }
    printf("%s mismatched bytes: %u/%zu\n", label, count, bytes);
    return count == 0u ? 0 : -1;
}

int main(void)
{
    DpInitialConditions input;
    double expected_y[DP_STATE_DIM];
    double expected_t;
    double expected_step;
    double expected_time[6];
    double expected_j[9];
    double expected_j_inv[9];
    double expected_mass;
    double expected_h[3];
    double expected_l[3];
    double expected_f[3];
    DpSadaRecovered pre_sada;
    DpSadaRecovered expected_sada;
    double time_array[6];
    int mismatch = 0;

    if (read_exact("pre_init_e8.bin", &input, sizeof(input)) != 0 ||
        read_exact("post_y_33.bin", expected_y, sizeof(expected_y)) != 0 ||
        read_exact("post_t.bin", &expected_t, sizeof(expected_t)) != 0 ||
        read_exact("post_step_time.bin", &expected_step, sizeof(expected_step)) != 0 ||
        read_exact("post_time_array_6.bin", expected_time, sizeof(expected_time)) != 0 ||
        read_exact("post_j_c_b_9.bin", expected_j, sizeof(expected_j)) != 0 ||
        read_exact("post_j_c_b_inv_9.bin", expected_j_inv, sizeof(expected_j_inv)) != 0 ||
        read_exact("post_spacecraft_mass.bin", &expected_mass, sizeof(expected_mass)) != 0 ||
        read_exact("post_h_w_b_3.bin", expected_h, sizeof(expected_h)) != 0 ||
        read_exact("post_l_c_b_3.bin", expected_l, sizeof(expected_l)) != 0 ||
        read_exact("post_f_i_external_3.bin", expected_f, sizeof(expected_f)) != 0 ||
        read_exact("pre_sada_104.bin", &pre_sada, sizeof(pre_sada)) != 0 ||
        read_exact("post_sada_104.bin", &expected_sada, sizeof(expected_sada)) != 0) {
        return 1;
    }

    SADA = pre_sada;
    DynamicInit(&input);
    TimeArrayGet(time_array);

    mismatch |= compare_blob("y", y, expected_y, sizeof(y));
    mismatch |= compare_blob("t", &t, &expected_t, sizeof(t));
    mismatch |= compare_blob("step_time", &step_time, &expected_step, sizeof(step_time));
    mismatch |= compare_blob("time_array", time_array, expected_time, sizeof(time_array));
    mismatch |= compare_blob("J_c_B", J_c_B_mem, expected_j, sizeof(J_c_B_mem));
    mismatch |= compare_blob("J_c_B_inv", J_c_B_inv_mem, expected_j_inv, sizeof(J_c_B_inv_mem));
    mismatch |= compare_blob("SpacecraftMass", &SpacecraftMass, &expected_mass,
                             sizeof(SpacecraftMass));
    mismatch |= compare_blob("H_w_B", H_w_B_mem, expected_h, sizeof(H_w_B_mem));
    mismatch |= compare_blob("L_c_B", L_c_B_mem, expected_l, sizeof(L_c_B_mem));
    mismatch |= compare_blob("F_I_external", F_I_external.data, expected_f, sizeof(expected_f));
    mismatch |= compare_blob("SADA", &SADA, &expected_sada, sizeof(SADA));
    return mismatch == 0 ? 0 : 1;
}

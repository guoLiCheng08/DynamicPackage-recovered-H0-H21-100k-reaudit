#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_core_layout.h"
#include "dynamic_satellite_globals.h"
#include "dynamic_time.h"

#ifndef GOLD
#define GOLD "analysis/golden/core_dynamic_global_abi/controlled_input/"
#endif

static int read_exact(const char *name, void *dst, size_t bytes)
{
    char path[512];
    FILE *file;
    size_t got;
    (void)snprintf(path, sizeof(path), "%s%s", GOLD, name);
    file = fopen(path, "rb");
    if (file == NULL) { perror(path); return -1; }
    got = fread(dst, 1u, bytes, file);
    if (fclose(file) != 0 || got != bytes) return -1;
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
            if (count < 8u) {
                const size_t slot = index / sizeof(double);
                uint64_t actual_bits = 0u;
                uint64_t expected_bits = 0u;
                memcpy(&actual_bits, a + slot * sizeof(double), sizeof(actual_bits));
                memcpy(&expected_bits, e + slot * sizeof(double), sizeof(expected_bits));
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

static DpMatrix sat_matrix(size_t offset)
{
    DpMatrix matrix = {0};
    memcpy(&matrix, &Sat.raw[offset], sizeof(matrix));
    return matrix;
}

static int read_matrix(const char *name, DpMatrix matrix)
{
    return read_exact(name, matrix.data, (size_t)matrix.rows * (size_t)matrix.row_stride *
                                     sizeof(double));
}

int main(void)
{
    DpCoreDynInput input;
    unsigned char output[0x148];
    unsigned char expected_output[0x148];
    double expected_y[DP_STATE_DIM];
    double expected_h[3], expected_l[3], expected_f[3];
    DpMatrix m4a8, m688, coupling, m9, ma, md;

    if (dp_differential_equation_global_reset() != 0) return 1;
    TimeInit(2025.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    m4a8 = sat_matrix(0x4a8u); m688 = sat_matrix(0x688u); coupling = sat_matrix(0x898u);
    m9 = sat_matrix(0x9a0u); ma = sat_matrix(0xa00u); md = sat_matrix(0xd38u);
    if (read_exact("pre_input_90.bin", &input, sizeof(input)) ||
        read_exact("pre_y_33.bin", y, sizeof(y)) || read_exact("pre_t.bin", &t, sizeof(t)) ||
        read_exact("step_time.bin", &step_time, sizeof(step_time)) ||
        read_exact("j_c_b_data.bin", J_c_B_mem, sizeof(J_c_B_mem)) ||
        read_exact("j_c_b_inv_data.bin", J_c_B_inv_mem, sizeof(J_c_B_inv_mem)) ||
        read_exact("h_w_b_data.bin", H_w_B_mem, sizeof(H_w_B_mem)) ||
        read_exact("l_c_b_data.bin", L_c_B_mem, sizeof(L_c_B_mem)) ||
        read_exact("f_i_external_data.bin", F_I_external.data, sizeof(double) * 3u) ||
        read_exact("spacecraft_mass.bin", &SpacecraftMass, sizeof(SpacecraftMass)) ||
        read_exact("sada_raw.bin", &SADA, sizeof(SADA)) ||
        read_matrix("sat_m4a8_data.bin", m4a8) || read_matrix("sat_m688_data.bin", m688) ||
        read_matrix("sat_coupling_data.bin", coupling) || read_matrix("sat_m9_data.bin", m9) ||
        read_matrix("sat_modal_a_data.bin", ma) || read_matrix("sat_modal_d_data.bin", md) ||
        read_exact("post_y_33.bin", expected_y, sizeof(expected_y)) ||
        read_exact("post_h_w_b_data.bin", expected_h, sizeof(expected_h)) ||
        read_exact("post_l_c_b_data.bin", expected_l, sizeof(expected_l)) ||
        read_exact("post_f_i_external_data.bin", expected_f, sizeof(expected_f)) ||
        read_exact("post_output_148.bin", expected_output, sizeof(expected_output))) return 1;
    int mismatch = 0;

    CoreDynamic(output, &input);
    mismatch |= compare_blob("y", y, expected_y, sizeof(y));
    mismatch |= compare_blob("H_w_B", H_w_B_mem, expected_h, sizeof(expected_h));
    mismatch |= compare_blob("L_c_B", L_c_B_mem, expected_l, sizeof(expected_l));
    mismatch |= compare_blob("F_I_external", F_I_external.data, expected_f, sizeof(expected_f));
    mismatch |= compare_blob("output", output, expected_output, sizeof(output));
    return mismatch == 0 ? 0 : 1;
}

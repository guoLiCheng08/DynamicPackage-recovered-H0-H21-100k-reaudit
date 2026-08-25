#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"
#include "dynamic_time.h"

#define DP_GLOBAL_GOLD_DIR "analysis/golden/differential_equation_global_abi/"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static int read_exact(const char *file_name, void *buffer, size_t byte_count)
{
    char path[512];
    FILE *input;
    size_t actual;

    if (snprintf(path, sizeof(path), "%s%s", DP_GLOBAL_GOLD_DIR, file_name) < 0) {
        return -1;
    }
    input = fopen(path, "rb");
    if (input == NULL) {
        printf("FAIL unable to open gold %s\n", path);
        return -1;
    }
    actual = fread(buffer, 1u, byte_count, input);
    if (actual != byte_count || fgetc(input) != EOF) {
        printf("FAIL invalid gold size %s expected=%zu actual=%zu\n",
               path, byte_count, actual);
        (void)fclose(input);
        return -1;
    }
    (void)fclose(input);
    return 0;
}

static int read_matrix_payload(const char *file_name, DpMatrix matrix)
{
    size_t element_count;

    if (matrix.rows <= 0 || matrix.cols <= 0 || matrix.row_stride < matrix.cols ||
        matrix.data == NULL) {
        return -1;
    }
    element_count = (size_t)matrix.rows * (size_t)matrix.row_stride;
    return read_exact(file_name, matrix.data, element_count * sizeof(double));
}

static int restore_global_snapshot(double state[DP_STATE_DIM],
                                   double expected[DP_STATE_DIM])
{
    DpMatrix coupling;
    DpMatrix modal_a;
    DpMatrix modal_d;
    DpMatrix m4a8;
    DpMatrix m688;
    DpMatrix m9a0;

    if (dp_differential_equation_global_reset() != 0) {
        puts("FAIL global differential reset");
        return -1;
    }
    TimeInit(2020.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    coupling = *(DpMatrix *)&Sat.raw[0x898u];
    modal_a = *(DpMatrix *)&Sat.raw[0xa00u];
    modal_d = *(DpMatrix *)&Sat.raw[0xd38u];
    m4a8 = *(DpMatrix *)&Sat.raw[0x4a8u];
    m688 = *(DpMatrix *)&Sat.raw[0x688u];
    m9a0 = *(DpMatrix *)&Sat.raw[0x9a0u];

    if (read_exact("state_33.bin", state, sizeof(double) * DP_STATE_DIM) != 0 ||
        read_exact("derivative_33.bin", expected, sizeof(double) * DP_STATE_DIM) != 0 ||
        read_exact("j_c_b_data.bin", J_c_B_mem, sizeof(J_c_B_mem)) != 0 ||
        read_exact("j_c_b_inv_data.bin", J_c_B_inv_mem, sizeof(J_c_B_inv_mem)) != 0 ||
        read_exact("h_w_b_data.bin", H_w_B_mem, sizeof(H_w_B_mem)) != 0 ||
        read_exact("l_c_b_data.bin", L_c_B_mem, sizeof(L_c_B_mem)) != 0 ||
        read_exact("f_i_external_data.bin", F_I_external.data,
                   sizeof(double) * 3u) != 0 ||
        read_exact("spacecraft_mass.bin", &SpacecraftMass,
                   sizeof(SpacecraftMass)) != 0 ||
        read_exact("sada_raw.bin", &SADA, sizeof(SADA)) != 0 ||
        read_matrix_payload("sat_coupling_data.bin", coupling) != 0 ||
        read_matrix_payload("sat_modal_a_data.bin", modal_a) != 0 ||
        read_matrix_payload("sat_modal_d_data.bin", modal_d) != 0 ||
        read_matrix_payload("sat_m4a8_data.bin", m4a8) != 0 ||
        read_matrix_payload("sat_m688_data.bin", m688) != 0 ||
        read_matrix_payload("sat_m9_data.bin", m9a0) != 0) {
        return -1;
    }
    return 0;
}

int main(void)
{
    double state[DP_STATE_DIM];
    double original_state[DP_STATE_DIM];
    double expected[DP_STATE_DIM];
    double actual[DP_STATE_DIM];
    unsigned mismatch_count = 0u;
    unsigned index;

    if (restore_global_snapshot(state, expected) != 0) {
        return 1;
    }
    memcpy(original_state, state, sizeof(state));
    differential_equation(actual, state);

    for (index = 0u; index < DP_STATE_DIM; ++index) {
        if (bits(actual[index]) != bits(expected[index])) {
            printf("DIFF derivative index=%u actual=%a expected_bits=%016" PRIx64
                   " actual_bits=%016" PRIx64 "\n",
                   index, actual[index], bits(expected[index]), bits(actual[index]));
            ++mismatch_count;
        }
        if (bits(state[index]) != bits(original_state[index])) {
            printf("DIFF input-mutated index=%u before=%016" PRIx64
                   " after=%016" PRIx64 "\n",
                   index, bits(original_state[index]), bits(state[index]));
            ++mismatch_count;
        }
    }
    if (mismatch_count != 0u) {
        printf("differential_equation global ABI original-ELF gold: FAIL mismatches=%u\n",
               mismatch_count);
        return 1;
    }
    puts("differential_equation global ABI original-ELF gold: PASS (33/33 bitwise; input preserved)");
    return 0;
}

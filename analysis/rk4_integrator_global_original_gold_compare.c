#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"
#include "dynamic_time.h"

#define DP_RK4_GLOBAL_GOLD_DIR "analysis/golden/rk4_integrator_global_abi/"

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

    if (snprintf(path, sizeof(path), "%s%s", DP_RK4_GLOBAL_GOLD_DIR, file_name) < 0) {
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

static DpMatrix sat_matrix_load(size_t offset)
{
    DpMatrix matrix = {0};
    memcpy(&matrix, &Sat.raw[offset], sizeof(matrix));
    return matrix;
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

typedef struct {
    double h_w_b[3];
    double l_c_b[3];
    double j_c_b[9];
    double j_c_b_inv[9];
    double f_i_external[3];
    double mass;
    double configured_step_time;
    DpSadaRecovered sada;
} Rk4GlobalInputs;

static int restore_global_snapshot(Rk4GlobalInputs *inputs, double *step)
{
    DpMatrix coupling;
    DpMatrix modal_a;
    DpMatrix modal_d;
    DpMatrix m4a8;
    DpMatrix m688;
    DpMatrix m9a0;

    if (inputs == NULL || step == NULL || dp_differential_equation_global_reset() != 0) {
        puts("FAIL RK4 global reset");
        return -1;
    }
    TimeInit(2020.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    coupling = sat_matrix_load(0x898u);
    modal_a = sat_matrix_load(0xa00u);
    modal_d = sat_matrix_load(0xd38u);
    m4a8 = sat_matrix_load(0x4a8u);
    m688 = sat_matrix_load(0x688u);
    m9a0 = sat_matrix_load(0x9a0u);

    if (read_exact("pre_state_33.bin", y, sizeof(y)) != 0 ||
        read_exact("pre_t.bin", &t, sizeof(t)) != 0 ||
        read_exact("explicit_step.bin", step, sizeof(*step)) != 0 ||
        read_exact("step_time.bin", &step_time, sizeof(step_time)) != 0 ||
        read_exact("h_w_b_data.bin", H_w_B_mem, sizeof(H_w_B_mem)) != 0 ||
        read_exact("l_c_b_data.bin", L_c_B_mem, sizeof(L_c_B_mem)) != 0 ||
        read_exact("j_c_b_data.bin", J_c_B_mem, sizeof(J_c_B_mem)) != 0 ||
        read_exact("j_c_b_inv_data.bin", J_c_B_inv_mem, sizeof(J_c_B_inv_mem)) != 0 ||
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

    memcpy(inputs->h_w_b, H_w_B_mem, sizeof(inputs->h_w_b));
    memcpy(inputs->l_c_b, L_c_B_mem, sizeof(inputs->l_c_b));
    memcpy(inputs->j_c_b, J_c_B_mem, sizeof(inputs->j_c_b));
    memcpy(inputs->j_c_b_inv, J_c_B_inv_mem, sizeof(inputs->j_c_b_inv));
    memcpy(inputs->f_i_external, F_I_external.data, sizeof(inputs->f_i_external));
    inputs->mass = SpacecraftMass;
    inputs->configured_step_time = step_time;
    inputs->sada = SADA;
    return 0;
}

static int compare_state(unsigned step_index, const char *file_name)
{
    double expected[DP_STATE_DIM];
    unsigned index;
    unsigned mismatch_count = 0u;

    if (read_exact(file_name, expected, sizeof(expected)) != 0) {
        return -1;
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        if (bits(y[index]) != bits(expected[index])) {
            printf("DIFF step=%u y[%u] actual=%a expected_bits=%016" PRIx64
                   " actual_bits=%016" PRIx64 "\n",
                   step_index, index, y[index], bits(expected[index]), bits(y[index]));
            ++mismatch_count;
        }
    }
    return mismatch_count == 0u ? 0 : -1;
}

static int compare_time(unsigned step_index, const char *file_name)
{
    double expected;

    if (read_exact(file_name, &expected, sizeof(expected)) != 0) {
        return -1;
    }
    if (bits(t) != bits(expected)) {
        printf("DIFF step=%u t actual=%a expected_bits=%016" PRIx64
               " actual_bits=%016" PRIx64 "\n",
               step_index, t, bits(expected), bits(t));
        return -1;
    }
    return 0;
}

static int compare_immutable_globals(const Rk4GlobalInputs *inputs)
{
    if (inputs == NULL ||
        memcmp(H_w_B_mem, inputs->h_w_b, sizeof(inputs->h_w_b)) != 0 ||
        memcmp(L_c_B_mem, inputs->l_c_b, sizeof(inputs->l_c_b)) != 0 ||
        memcmp(J_c_B_mem, inputs->j_c_b, sizeof(inputs->j_c_b)) != 0 ||
        memcmp(J_c_B_inv_mem, inputs->j_c_b_inv, sizeof(inputs->j_c_b_inv)) != 0 ||
        memcmp(F_I_external.data, inputs->f_i_external, sizeof(inputs->f_i_external)) != 0 ||
        bits(SpacecraftMass) != bits(inputs->mass) ||
        bits(step_time) != bits(inputs->configured_step_time) ||
        memcmp(&SADA, &inputs->sada, sizeof(SADA)) != 0) {
        puts("DIFF RK4 unexpectedly changed an input global payload");
        return -1;
    }
    return 0;
}

int main(void)
{
    Rk4GlobalInputs inputs;
    double step;

    if (restore_global_snapshot(&inputs, &step) != 0) {
        return 1;
    }
    RK4_Intergrator(step);
    if (compare_state(1u, "step1_state_33.bin") != 0 ||
        compare_time(1u, "step1_t.bin") != 0 ||
        compare_immutable_globals(&inputs) != 0) {
        return 1;
    }

    RK4_Intergrator(step);
    if (compare_state(2u, "step2_state_33.bin") != 0 ||
        compare_time(2u, "step2_t.bin") != 0 ||
        compare_immutable_globals(&inputs) != 0) {
        return 1;
    }
    puts("RK4_Intergrator global ABI original-ELF gold: PASS (2 steps, y/t bitwise; globals preserved)");
    return 0;
}

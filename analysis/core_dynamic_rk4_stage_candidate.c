#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"
#include "dynamic_time.h"

#define GOLD "analysis/golden/core_dynamic_global_abi/controlled_input/"

static unsigned stage_count;
static double captured_derivative[4][DP_STATE_DIM];

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

static DpMatrix sat_matrix(size_t offset)
{
    DpMatrix matrix = {0};
    memcpy(&matrix, &Sat.raw[offset], sizeof(matrix));
    return matrix;
}

static int read_matrix(const char *name, DpMatrix matrix)
{
    return read_exact(name, matrix.data,
                      (size_t)matrix.rows * (size_t)matrix.row_stride * sizeof(double));
}

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static void stage_derivative(double ignored_time, const double state[DP_STATE_DIM],
                             double derivative[DP_STATE_DIM], void *ignored)
{
    (void)ignored_time;
    (void)ignored;
    printf("candidate_stage=%u q=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64
           " w=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64
           " r=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64
           " v=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64
           " flex=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64
           ",%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64
           ",%016" PRIx64 ",%016" PRIx64 "\n",
           stage_count, bits(state[0]), bits(state[1]), bits(state[2]), bits(state[3]),
           bits(state[4]), bits(state[5]), bits(state[6]), bits(state[7]), bits(state[8]),
           bits(state[9]), bits(state[10]), bits(state[11]), bits(state[12]),
           bits(state[13]), bits(state[14]), bits(state[15]), bits(state[16]),
           bits(state[17]), bits(state[18]), bits(state[19]), bits(state[20]),
           bits(state[21]), bits(state[22]));
    differential_equation(derivative, state);
    if (stage_count < 4u) {
        memcpy(captured_derivative[stage_count], derivative, sizeof(captured_derivative[0]));
    }
    printf("candidate_derivative_stage=%u alpha=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64
           " eta_ddot=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64
           ",%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64
           ",%016" PRIx64 ",%016" PRIx64 "\n",
           stage_count, bits(derivative[4]), bits(derivative[5]), bits(derivative[6]),
           bits(derivative[13]), bits(derivative[14]), bits(derivative[15]),
           bits(derivative[16]), bits(derivative[17]), bits(derivative[18]),
           bits(derivative[19]), bits(derivative[20]), bits(derivative[21]),
           bits(derivative[22]));
    ++stage_count;
}

int main(void)
{
#ifdef DP_USE_GLOBAL_RK4
    (void)stage_derivative;
#endif
    DpMatrix m448, m4a8, m688, coupling, m9, ma, md;
    double next_time;

    if (dp_differential_equation_global_reset() != 0) return 1;
    TimeInit(2025.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    m448 = sat_matrix(0x448u); m4a8 = sat_matrix(0x4a8u); m688 = sat_matrix(0x688u); coupling = sat_matrix(0x898u);
    m9 = sat_matrix(0x9a0u); ma = sat_matrix(0xa00u); md = sat_matrix(0xd38u);
    if (read_exact("pre_y_33.bin", y, sizeof(y)) ||
        read_exact("pre_t.bin", &t, sizeof(t)) ||
        read_exact("step_time.bin", &step_time, sizeof(step_time)) ||
        read_exact("j_c_b_data.bin", J_c_B_mem, sizeof(J_c_B_mem)) ||
        read_exact("j_c_b_inv_data.bin", J_c_B_inv_mem, sizeof(J_c_B_inv_mem)) ||
        read_exact("post_h_w_b_data.bin", H_w_B_mem, sizeof(H_w_B_mem)) ||
        read_exact("post_l_c_b_data.bin", L_c_B_mem, sizeof(L_c_B_mem)) ||
        read_exact("post_f_i_external_data.bin", F_I_external.data, 3u * sizeof(double)) ||
        read_exact("spacecraft_mass.bin", &SpacecraftMass, sizeof(SpacecraftMass)) ||
        read_exact("sada_raw.bin", &SADA, sizeof(SADA)) ||
        read_matrix("sat_m448_data.bin", m448) || read_matrix("sat_m4a8_data.bin", m4a8) ||
        read_matrix("sat_m688_data.bin", m688) ||
        read_matrix("sat_coupling_data.bin", coupling) || read_matrix("sat_m9_data.bin", m9) ||
        read_matrix("sat_modal_a_data.bin", ma) || read_matrix("sat_modal_d_data.bin", md)) {
        return 1;
    }
    next_time = t;
#ifdef DP_USE_GLOBAL_RK4
    (void)next_time;
    RK4_Intergrator(step_time);
    stage_count = 4u;
#else
    dp_rk4_step_33(y, &next_time, step_time, stage_derivative, NULL);
    {
        const size_t index = 18u;
        const double h_k1 = captured_derivative[0][index] * step_time;
        const double h_k2 = captured_derivative[1][index] * step_time;
        const double h_k3 = captured_derivative[2][index] * step_time;
        const double h_k4 = captured_derivative[3][index] * step_time;
        double weighted = h_k4;
        printf("candidate_hk_y18=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 "\n",
               bits(h_k1), bits(h_k2), bits(h_k3), bits(h_k4));
        weighted += h_k3 + h_k3;
        weighted += h_k2 + h_k2;
        weighted += h_k1;
        printf("candidate_weighted_y18=%016" PRIx64 "\n", bits(weighted));
    }
#endif
    printf("candidate_final_y18=%016" PRIx64 "\n", bits(y[18]));
    printf("candidate_stage_count=%u\n", stage_count);
    return stage_count == 4u ? 0 : 1;
}

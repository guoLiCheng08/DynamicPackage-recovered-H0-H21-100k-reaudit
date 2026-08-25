#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"
#include "dynamic_time.h"

#define GOLD "analysis/golden/core_dynamic_global_abi/controlled_input/"

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

static void print3(const char *label, const double v[3])
{
    printf("%s=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 "\n",
           label, bits(v[0]), bits(v[1]), bits(v[2]));
}

int main(void)
{
    DpMatrix m4, m9, coupling, ma, md;
    double rate_data[3];
    double eta_data[10];
    double eta_dot_data[10];
    double h_data[3] = {0.0, 0.0, 0.0};
    double c_eta_data[3] = {0.0, 0.0, 0.0};
    double sada_in_data[3] = {0.0, 0.0, 0.0};
    double sada_pre_data[3] = {0.0, 0.0, 0.0};
    double sada_h_data[3] = {0.0, 0.0, 0.0};
    double gyro_data[3] = {0.0, 0.0, 0.0};
    double l_data[3];
    double ma_eta_data[10] = {0.0};
    double md_eta_dot_data[10] = {0.0};
    double stiff_data[3] = {0.0, 0.0, 0.0};
    double damp_data[3] = {0.0, 0.0, 0.0};
    double rhs_data[3] = {0.0, 0.0, 0.0};
    DpVector rate = {3, 0, rate_data};
    DpVector eta = {10, 0, eta_data};
    DpVector eta_dot = {10, 0, eta_dot_data};
    DpVector h = {3, 0, h_data};
    DpVector c_eta = {3, 0, c_eta_data};
    DpVector sada_in = {3, 0, sada_in_data};
    DpVector sada_pre = {3, 0, sada_pre_data};
    DpVector sada_h = {3, 0, sada_h_data};
    DpVector gyro = {3, 0, gyro_data};
    DpVector ma_eta = {10, 0, ma_eta_data};
    DpVector md_eta_dot = {10, 0, md_eta_dot_data};
    DpVector stiff = {3, 0, stiff_data};
    DpVector damp = {3, 0, damp_data};

    if (dp_differential_equation_global_reset() != 0) return 1;
    TimeInit(2025.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    m4 = sat_matrix(0x4a8u); m9 = sat_matrix(0x9a0u); coupling = sat_matrix(0x898u);
    ma = sat_matrix(0xa00u); md = sat_matrix(0xd38u);
    if (read_exact("pre_y_33.bin", y, sizeof(y)) ||
        read_exact("j_c_b_data.bin", J_c_B_mem, sizeof(J_c_B_mem)) ||
        read_exact("post_h_w_b_data.bin", H_w_B_mem, sizeof(H_w_B_mem)) ||
        read_exact("post_l_c_b_data.bin", l_data, sizeof(l_data)) ||
        read_exact("sada_raw.bin", &SADA, sizeof(SADA)) ||
        read_matrix("sat_m4a8_data.bin", m4) || read_matrix("sat_m9_data.bin", m9) ||
        read_matrix("sat_coupling_data.bin", coupling) || read_matrix("sat_modal_a_data.bin", ma) ||
        read_matrix("sat_modal_d_data.bin", md)) return 1;

    memcpy(rate_data, &y[4], sizeof(rate_data));
    memcpy(eta_data, &y[13], sizeof(eta_data));
    memcpy(eta_dot_data, &y[23], sizeof(eta_dot_data));
    sada_in_data[1] = SADA.command_angle[0];
    sada_in_data[2] = SADA.command_angle[1];
    if (blas_gemv(&J_c_B, &rate, &h, 1.0, 0.0) != 0 ||
        blas_gemv(&coupling, &eta, &c_eta, 1.0, 0.0) != 0 ||
        vector_add(&h, &c_eta) != 0 ||
        blas_gemv(&m4, &sada_in, &sada_pre, 1.0, 0.0) != 0 ||
        blas_gemv(&m9, &sada_pre, &sada_h, 1.0, 0.0) != 0 ||
        vector_add(&h, &sada_h) != 0 ||
        vector_add(&h, &H_w_B) != 0 ||
        vector3_cross(&rate, &h, &gyro) != 0 ||
        blas_gemv(&ma, &eta, &ma_eta, 1.0, 0.0) != 0 ||
        blas_gemv(&coupling, &ma_eta, &stiff, 1.0, 0.0) != 0 ||
        blas_gemv(&md, &eta_dot, &md_eta_dot, 1.0, 0.0) != 0 ||
        blas_gemv(&coupling, &md_eta_dot, &damp, 1.0, 0.0) != 0) return 1;
    stiff_data[0] *= 0.1; stiff_data[1] *= 0.1; stiff_data[2] *= 0.1;
    rhs_data[0] = -gyro_data[0] - l_data[0] + stiff_data[0] + damp_data[0];
    rhs_data[1] = -gyro_data[1] - l_data[1] + stiff_data[1] + damp_data[1];
    rhs_data[2] = -gyro_data[2] - l_data[2] + stiff_data[2] + damp_data[2];
    print3("candidate_h_total", h_data);
    print3("candidate_gyro", gyro_data);
    print3("candidate_l", l_data);
    print3("candidate_stiff", stiff_data);
    print3("candidate_damp", damp_data);
    print3("candidate_rhs", rhs_data);
    return 0;
}

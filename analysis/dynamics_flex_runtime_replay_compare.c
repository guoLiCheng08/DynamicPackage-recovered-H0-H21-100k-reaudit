#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

#define SAT_M448 0x448u
#define SAT_M4A8 0x4a8u
#define SAT_M688 0x688u
#define SAT_C 0x898u
#define SAT_M9A0 0x9a0u
#define SAT_A 0xa00u
#define SAT_D 0xd38u

static int load_block(const char *path, double *out, size_t count)
{
    FILE *file = fopen(path, "rb");
    int ok;

    if (file == NULL) return -1;
    ok = fread(out, sizeof(*out), count, file) == count && fgetc(file) == EOF;
    (void)fclose(file);
    return ok ? 0 : -1;
}

static void store_matrix(size_t offset, DpMatrix matrix)
{
    memcpy(&Sat.raw[offset], &matrix, sizeof(matrix));
}

static int first_mismatch(const double *actual, const uint64_t *expected,
                          size_t count, const char *label)
{
    size_t i;
    uint64_t bits;

    for (i = 0u; i < count; ++i) {
        memcpy(&bits, &actual[i], sizeof(bits));
        if (bits != expected[i]) {
            (void)printf("%s[%zu] actual=%016llx expected=%016llx\n", label, i,
                         (unsigned long long)bits,
                         (unsigned long long)expected[i]);
            return -1;
        }
    }
    return 0;
}

int main(void)
{
    double m448_data[9];
    double m4a8_data[9];
    double m688_data[30];
    double coupling_data[30];
    double m9a0_data[9];
    double modal_a_data[100];
    double modal_d_data[100];
    double alpha_data[3] = {0.1, -0.2, 0.3};
    double rate_data[3] = {1.0, 2.0, -1.0};
    double acc_data[10] = {0.0};
    double position_data[10] = {0.25, -0.5};
    double copied_position_data[10] = {0.0};
    double velocity_data[10] = {0.75, 0.125};
    static const uint64_t original_j_bits[9] = {
        UINT64_C(0x40924a0000000000), UINT64_C(0x4088700000000000), UINT64_C(0x3ff7ae147ae147ae),
        UINT64_C(0x40886fffffffffff), UINT64_C(0x4094b9ffffffffff), UINT64_C(0xc031b33333333333),
        UINT64_C(0x3ff7ae147ae147ae), UINT64_C(0xc031b33333333333), UINT64_C(0x40a02e0000000000)
    };
    static const uint64_t expected_alpha[3] = {
        UINT64_C(0x3fe145bcd43bcf9e), UINT64_C(0x3fc50eca928bb920), UINT64_C(0x3fef9609d65fd7fc)
    };
    static const uint64_t expected_acc[10] = {
        UINT64_C(0xbffae38c15ff7e43), UINT64_C(0x4013de9150fb634e),
        UINT64_C(0x3fb254fc16a32413), UINT64_C(0x3fe9a68c5e517ea5),
        UINT64_C(0x3fdd06b375184899), UINT64_C(0xbfcd42c5b094eb02),
        UINT64_C(0x3fc95ea2c36afbd3), UINT64_C(0x3f6c322602de1713),
        UINT64_C(0x3fb4599304cae459), UINT64_C(0x3f9694915f71ab21)
    };
    static const uint64_t expected_copy[10] = {
        UINT64_C(0x3fd0000000000000), UINT64_C(0xbfe0000000000000),
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u
    };
    DpVector alpha = {3, 0, alpha_data};
    DpVector rate = {3, 0, rate_data};
    DpVector acc = {10, 0, acc_data};
    DpVector position = {10, 0, position_data};
    DpVector copied_position = {10, 0, copied_position_data};
    DpVector velocity = {10, 0, velocity_data};

    if (load_block("analysis/time_orbit/dynamics_flex_sat_448.bin", m448_data, 9u) != 0 ||
        load_block("analysis/time_orbit/dynamics_flex_sat_4a8.bin", m4a8_data, 9u) != 0 ||
        load_block("analysis/time_orbit/dynamics_flex_sat_688.bin", m688_data, 30u) != 0 ||
        load_block("analysis/time_orbit/dynamics_flex_sat_898.bin", coupling_data, 30u) != 0 ||
        load_block("analysis/time_orbit/dynamics_flex_sat_9a0.bin", m9a0_data, 9u) != 0 ||
        load_block("analysis/time_orbit/dynamics_flex_sat_a00.bin", modal_a_data, 100u) != 0 ||
        load_block("analysis/time_orbit/dynamics_flex_sat_d38.bin", modal_d_data, 100u) != 0) {
        return 2;
    }

    Update_sat_inertia_xw(0);
    memcpy(J_c_B_mem, original_j_bits, sizeof(original_j_bits));
    store_matrix(SAT_M448, (DpMatrix){3, 3, 3, 0, m448_data});
    store_matrix(SAT_M4A8, (DpMatrix){3, 3, 3, 0, m4a8_data});
    store_matrix(SAT_M688, (DpMatrix){3, 10, 10, 0, m688_data});
    store_matrix(SAT_C, (DpMatrix){3, 10, 10, 0, coupling_data});
    store_matrix(SAT_M9A0, (DpMatrix){3, 3, 3, 0, m9a0_data});
    store_matrix(SAT_A, (DpMatrix){10, 10, 10, 0, modal_a_data});
    store_matrix(SAT_D, (DpMatrix){10, 10, 10, 0, modal_d_data});
    SADA.command_angle[0] = 0.0;
    SADA.command_angle[1] = 0.0;
    SADA.angular_acceleration[0] = 0.0;
    SADA.angular_acceleration[1] = 0.0;

    dynamics_flex(&alpha, &rate, &acc, &position, &copied_position, &velocity,
                  &L_c_B, &H_w_B, &J_c_B, &J_c_B_inv);
    if (first_mismatch(alpha_data, expected_alpha, 3u, "alpha") != 0 ||
        first_mismatch(acc_data, expected_acc, 10u, "modal_acc") != 0 ||
        first_mismatch(copied_position_data, expected_copy, 10u, "copied_position") != 0) {
        return 1;
    }
    (void)puts("PASS: dynamics_flex runtime backing replay");
    return 0;
}

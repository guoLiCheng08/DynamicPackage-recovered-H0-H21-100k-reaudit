#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_dynamics.h"
#include "dynamic_time.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static double from_bits(uint64_t value)
{
    double result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    const uint64_t state_bits[DP_STATE_DIM] = {
        UINT64_C(0x3ff0000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000),
        UINT64_C(0x3f847ae147ae147b), UINT64_C(0xbf947ae147ae147b), UINT64_C(0x3f9eb851eb851eb8),
        UINT64_C(0x41509592644cbdf4), UINT64_C(0x4154d396bd85c590), UINT64_C(0x411b7a6b485f560f),
        UINT64_C(0xc0b718923f395fb3), UINT64_C(0x40b23870c2afc513), UINT64_C(0x4082044582e51745),
        UINT64_C(0x3f50624dd2f1a9fc), UINT64_C(0xbf60624dd2f1a9fc), UINT64_C(0x3f689374bc6a7efa), UINT64_C(0xbf70624dd2f1a9fc),
        UINT64_C(0x3f747ae147ae147b), UINT64_C(0xbf789374bc6a7efa), UINT64_C(0x3f7cac083126e979), UINT64_C(0xbf80624dd2f1a9fc),
        UINT64_C(0x3f826e978d4fdf3b), UINT64_C(0xbf847ae147ae147b),
        UINT64_C(0x3f86872b020c49ba), UINT64_C(0xbf889374bc6a7efa), UINT64_C(0x3f8a9fbe76c8b439), UINT64_C(0xbf8cac083126e979),
        UINT64_C(0x3f8eb851eb851eb8), UINT64_C(0xbf90624dd2f1a9fc), UINT64_C(0x3f916872b020c49c), UINT64_C(0xbf926e978d4fdf3b),
        UINT64_C(0x3f9374bc6a7ef9db), UINT64_C(0xbf947ae147ae147b)
    };
    const uint64_t expected_bits[DP_STATE_DIM] = {
        UINT64_C(0x0000000000000000), UINT64_C(0x3f747ae147ae147b), UINT64_C(0xbf847ae147ae147b), UINT64_C(0x3f8eb851eb851eb8),
        UINT64_C(0x3f39b6beecf70090), UINT64_C(0xbf1961bb2a67e6e3), UINT64_C(0x3f167e4052c1621f),
        UINT64_C(0xc0b718923f395fb3), UINT64_C(0x40b23870c2afc513), UINT64_C(0x4082044582e51745),
        UINT64_C(0xc0144c00f995ba53), UINT64_C(0xc0197d2304f7c0a0), UINT64_C(0xbfe0df15cb5811aa),
        UINT64_C(0xbfa105a61cec36c6), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000),
        UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000),
        UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000),
        UINT64_C(0x3f86872b020c49ba), UINT64_C(0xbf889374bc6a7efa), UINT64_C(0x3f8a9fbe76c8b439), UINT64_C(0xbf8cac083126e979),
        UINT64_C(0x3f8eb851eb851eb8), UINT64_C(0xbf90624dd2f1a9fc), UINT64_C(0x3f916872b020c49c), UINT64_C(0xbf926e978d4fdf3b),
        UINT64_C(0x3f9374bc6a7ef9db), UINT64_C(0xbf947ae147ae147b)
    };
    double state[DP_STATE_DIM];
    double derivative[DP_STATE_DIM];
    double c_data[30] = {0.0};
    double j_data[9] = {10.0, 0.0, 0.0, 0.0, 11.0, 0.0, 0.0, 0.0, 12.0};
    double ma_data[100] = {0.0};
    double md_data[100] = {0.0};
    DpMatrix c = {3, 10, 10, 0, c_data};
    DpMatrix j = {3, 3, 3, 0, j_data};
    DpMatrix ma = {10, 10, 10, 0, ma_data};
    DpMatrix md = {10, 10, 10, 0, md_data};
    DpVec3 momentum = {0.0, 0.0, 0.0};
    DpDynamicsContext context;
    unsigned index;

    c_data[0] = 0.1;
    c_data[10] = -0.05;
    c_data[20] = 0.025;
    ma_data[0] = 2.0;
    md_data[0] = 3.0;
    for (index = 0u; index < DP_STATE_DIM; ++index) state[index] = from_bits(state_bits[index]);
    TimeInit(2020.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    SpacecraftMass = 100.0;
    F_I_external.data[0] = 0.0;
    F_I_external.data[1] = 0.0;
    F_I_external.data[2] = 0.0;

    context.flex.rigid_matrices.coupling_3xn = c;
    context.flex.rigid_matrices.base_inertia_3x3 = j;
    context.flex.modal_matrix_a_nxn = &ma;
    context.flex.modal_matrix_d_nxn = &md;
    context.flex.angular_momentum = &momentum;
    context.flex.minus_term = NULL;
    context.flex.rigid_map_3x3 = NULL;
    context.flex.rigid_aux = NULL;

    if (dp_differential_equation_33(derivative, state, &context) != 0) {
        puts("FAIL recovered differential equation returned error");
        return 1;
    }
    unsigned mismatch_count = 0u;
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        if (bits(derivative[index]) != expected_bits[index]) {
            printf("DIFF index=%u actual=%a expected_bits=%016" PRIx64 " actual_bits=%016" PRIx64 "\n",
                   index, derivative[index], expected_bits[index], bits(derivative[index]));
            ++mismatch_count;
        }
    }
    if (mismatch_count != 0u) {
        printf("DIFF_SUMMARY count=%u\n", mismatch_count);
        return 1;
    }
    puts("differential equation nonzero C/K original-ELF compare: PASS (bitwise)");
    return 0;
}

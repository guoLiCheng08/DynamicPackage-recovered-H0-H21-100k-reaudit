#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_dynamics.h"
#include "dynamic_recovered.h"
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

static void recovered_derivative(double time, const double state[DP_STATE_DIM],
                                 double derivative[DP_STATE_DIM], void *opaque)
{
    const DpDynamicsContext *context = opaque;
    (void)time;
    if (dp_differential_equation_33(derivative, state, context) != 0) {
        fputs("FAIL dp_differential_equation_33 returned error in RK4 callback\n", stderr);
        abort();
    }
}

static int compare_state(unsigned step, const double actual[DP_STATE_DIM],
                         const uint64_t expected[DP_STATE_DIM])
{
    unsigned index;
    unsigned mismatch_count = 0u;
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        if (bits(actual[index]) != expected[index]) {
            printf("FAIL step=%u state[%u] actual=%a expected_bits=%016" PRIx64
                   " actual_bits=%016" PRIx64 "\n",
                   step, index, actual[index], expected[index], bits(actual[index]));
            ++mismatch_count;
        }
    }
    if (mismatch_count != 0u) {
        printf("FAIL step=%u mismatch_count=%u\n", step, mismatch_count);
        return 1;
    }
    return 0;
}

int main(void)
{
    const uint64_t initial_bits[DP_STATE_DIM] = {
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
    const uint64_t expected_step1[DP_STATE_DIM] = {
        UINT64_C(0x3feffffc53e5031e), UINT64_C(0x3f406ab8f0ec6e31), UINT64_C(0xbf50635aa5830887), UINT64_C(0x3f58945ff0005e6a),
        UINT64_C(0x3f84902d83eee109), UINT64_C(0xbf947d911677b9c9), UINT64_C(0x3f9ebaa689676f64),
        UINT64_C(0x415094fe92384cca), UINT64_C(0x4154d40b57e7035b), UINT64_C(0x411b7b51e2bf990b),
        UINT64_C(0xc0b7191423628791), UINT64_C(0x40b237cda007b205), UINT64_C(0x408203d986fb79df),
        UINT64_C(0xbf6429b0a6320b6b), UINT64_C(0xbf60624dd2f1a9fc), UINT64_C(0x3f689374bc6a7efa), UINT64_C(0xbf70624dd2f1a9fc),
        UINT64_C(0x3f747ae147ae147b), UINT64_C(0xbf789374bc6a7efa), UINT64_C(0x3f7cac083126e979), UINT64_C(0xbf80624dd2f1a9fc),
        UINT64_C(0x3f826e978d4fdf3b), UINT64_C(0xbf847ae147ae147b),
        UINT64_C(0x3f88e5b4b389be12), UINT64_C(0xbf8b292238392c72), UINT64_C(0x3f8d6c8fbce89ad0), UINT64_C(0xbf8faffd4198092f),
        UINT64_C(0x3f90f9b56323bbc7), UINT64_C(0xbf921b6c257b72f6), UINT64_C(0x3f933d22e7d32a26), UINT64_C(0xbf945ed9aa2ae155),
        UINT64_C(0x3f9580906c829884), UINT64_C(0xbf96a2472eda4fb4)
    };
    const uint64_t expected_step2[DP_STATE_DIM] = {
        UINT64_C(0x3feffff14d1f9eb2), UINT64_C(0x3f50738ac692e1cb), UINT64_C(0xbf60647a5b7e6250), UINT64_C(0x3f6895558388a732),
        UINT64_C(0x3f84a708e7f3bb6e), UINT64_C(0xbf948092a7a0e2c2), UINT64_C(0x3f9ebd27b59aec22),
        UINT64_C(0x4150946abce49c49), UINT64_C(0x4154d47fee3424c0), UINT64_C(0x411b7c3877b99352),
        UINT64_C(0xc0b719960304a6d0), UINT64_C(0x40b2372a79cfcdbf), UINT64_C(0x4082036d8788daa4),
        UINT64_C(0xbf7975ca29e997c4), UINT64_C(0xbf60624dd2f1a9fc), UINT64_C(0x3f689374bc6a7efa), UINT64_C(0xbf70624dd2f1a9fc),
        UINT64_C(0x3f747ae147ae147b), UINT64_C(0xbf789374bc6a7efa), UINT64_C(0x3f7cac083126e979), UINT64_C(0xbf80624dd2f1a9fc),
        UINT64_C(0x3f826e978d4fdf3b), UINT64_C(0xbf847ae147ae147b),
        UINT64_C(0x3f8b8408a88a6f0a), UINT64_C(0xbf8e046689513353), UINT64_C(0x3f904262350bfbcc), UINT64_C(0xbf918291256f5df0),
        UINT64_C(0x3f92c2c015d2c013), UINT64_C(0xbf9402ef06362236), UINT64_C(0x3f95431df699845a), UINT64_C(0xbf96834ce6fce67d),
        UINT64_C(0x3f97c37bd76048a0), UINT64_C(0xbf9903aac7c3aac4)
    };
    double state[DP_STATE_DIM];
    double time = 0.0;
    double c_data[30] = {0.0};
    double j_data[9] = {10.0, 0.0, 0.0, 0.0, 11.0, 0.0, 0.0, 0.0, 12.0};
    double ma_data[100] = {0.0};
    double md_data[100] = {0.0};
    DpMatrix c = {3, 10, 10, 0, c_data};
    DpMatrix j = {3, 3, 3, 0, j_data};
    DpMatrix ma = {10, 10, 10, 0, ma_data};
    DpMatrix md = {10, 10, 10, 0, md_data};
    DpVec3 actuator_momentum = {0.0, 0.0, 0.0};
    DpDynamicsContext context = {0};
    unsigned index;

    c_data[0] = 0.1;
    c_data[10] = -0.05;
    c_data[20] = 0.025;
    ma_data[0] = 2.0;
    md_data[0] = 3.0;
    for (index = 0u; index < DP_STATE_DIM; ++index) state[index] = from_bits(initial_bits[index]);
    TimeInit(2020.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    SpacecraftMass = 100.0;
    F_I_external.data[0] = 0.0;
    F_I_external.data[1] = 0.0;
    F_I_external.data[2] = 0.0;

    context.flex.rigid_matrices.coupling_3xn = c;
    context.flex.rigid_matrices.base_inertia_3x3 = j;
    context.flex.modal_matrix_a_nxn = &ma;
    context.flex.modal_matrix_d_nxn = &md;
    context.flex.angular_momentum = &actuator_momentum;
    context.flex.minus_term = NULL;
    context.flex.rigid_map_3x3 = NULL;
    context.flex.rigid_aux = NULL;

    dp_rk4_integrator_33(state, &time, 0.1, recovered_derivative, &context);
    if (bits(time) != UINT64_C(0x3fb999999999999a)) {
        printf("FAIL step=1 time expected_bits=3fb999999999999a actual_bits=%016" PRIx64 "\n", bits(time));
        return 1;
    }
    if (compare_state(1u, state, expected_step1) != 0) return 1;

    dp_rk4_integrator_33(state, &time, 0.1, recovered_derivative, &context);
    if (bits(time) != UINT64_C(0x3fc999999999999a)) {
        printf("FAIL step=2 time expected_bits=3fc999999999999a actual_bits=%016" PRIx64 "\n", bits(time));
        return 1;
    }
    if (compare_state(2u, state, expected_step2) != 0) return 1;

    puts("full-state two-step RK4 original-ELF gold compare: PASS (bitwise)");
    return 0;
}

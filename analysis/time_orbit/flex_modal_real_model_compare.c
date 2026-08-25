#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_flex.h"

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
    double c_data[30] = {
        0x1.081e515606fe7p+2, -0x1.faeaa91fe4a76p-4, -0x1.052a1ecb31455p-9, -0x1.9f5643a6ea2f7p-1, -0x1.56e0e2808e1f4p-1, -0x1.dbc666c119d64p-10, -0x1.46ae7d70dd636p-1, 0x1.4d5c5b9276341p-13, -0x1.9c4b63387aa19p-3, -0x1.d402f6f09ab80p-5,
        -0x1.0b21c3e8236dfp+2, 0x1.692da478ac675p-3, 0x1.734f76de9fcd9p-9, 0x1.463cdf4fd98c0p+0, -0x1.367f212da3db2p-1, 0x1.5319eada4c0c7p-9, 0x1.fee65b2a131f2p-1, -0x1.e13381d29449ep-13, 0x1.7dee41cb1333fp-3, 0x1.b7186df62db94p-5,
        0x1.7e6af29ac2fc1p-12, -0x1.4103e3476b2c2p+2, -0x1.26a04d232f963p-4, -0x1.2969fff8f41d2p-1, 0x1.ff88a59a47a9ep-8, 0x1.db7533dea9c41p-3, -0x1.2a331259a76f3p-6, -0x1.cf6d43addd510p-9, -0x1.90cfd7aa5c941p-10, -0x1.12aad7a377fedp-15,
    };
    double ma_data[100] = {
        0x1.a8a48e3f486a5p-2, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x1.51c8b6f9b7f89p-1, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x1.bf2967464f089p-1, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.69e956568a0bap+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.2992580f69322p+1, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.6b851cb2c6c4dp+1, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.85418269afcfap+1, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.faad127adf77dp+1, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.fc48d8d4f66f9p+1, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.13db623cf234fp+2,
    };
    double md_data[100] = {
        0x1.6030c080fddb2p-3, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x1.bdb20f201d45ap-2, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x1.8688ceac5c253p-1, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.ffa3ea2ecc351p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.59e4e3fa90d55p+2, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.0219624c5065ap+3, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.27f0138b199b8p+3, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.f568510759909p+3, 0x0.0p+0, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.f89898a2cea19p+3, 0x0.0p+0,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x1.2941110f46efdp+4,
    };
    double j_data[9] = {
        0x1.24a0000000000p+10, 0x1.8700000000000p+9, 0x1.7ae147ae147aep+0,
        0x1.86fffffffffffp+9, 0x1.4b9ffffffffffp+10, -0x1.1b33333333333p+4,
        0x1.7ae147ae147aep+0, -0x1.1b33333333333p+4, 0x1.02e0000000000p+11
    };
    double modal_position_data[10] = {
        0x1.0624dd2f1a9fcp-10, -0x1.0624dd2f1a9fcp-9, 0x1.89374bc6a7efap-9,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0
    };
    double modal_velocity_data[10] = {
        0x1.6872b020c49bap-7, -0x1.89374bc6a7efap-7, 0x1.a9fbe76c8b439p-7,
        0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0
    };
    double output_data[10] = {0.0};
    double modal_velocity_output_data[10] = {0.0};
    const uint64_t alpha_bits[3] = {
        UINT64_C(0x3f36e0882c67b83c), UINT64_C(0x3f36dcb2526b234e),
        UINT64_C(0x3f23567451d0a876)
    };
    const uint64_t expected[10] = {
        UINT64_C(0xbf5f6b5e4cb791b9), UINT64_C(0x3f78e3d0bd482e6e),
        UINT64_C(0xbf84d2c06cb74ba9), UINT64_C(0xbf13d737cf235946),
        UINT64_C(0x3f3d1c028f2d140e), UINT64_C(0xbf02193bb4a383d2),
        UINT64_C(0xbf20149e75bcf8ee), UINT64_C(0x3ea253bdcfcda598),
        UINT64_C(0x3ed6d5111c87b889), UINT64_C(0x3eb4f5831c2f9526)
    };
    const uint64_t rate_bits[3] = {
        UINT64_C(0x3f847ae147ae147b), UINT64_C(0xbf947ae147ae147b),
        UINT64_C(0x3f9eb851eb851eb8)
    };
    const uint64_t momentum_bits[3] = {
        UINT64_C(0xc00f17009913099e), UINT64_C(0xc0333eda470aa2eb),
        UINT64_C(0x404f411a56c06a96)
    };
    const uint64_t expected_rhs_bits[3] = {
        UINT64_C(0x3fe5ce3fcd7b4568), UINT64_C(0x3fe7722fe7e80661),
        UINT64_C(0x3fd2f6b61d53fb4f)
    };
    DpMatrix c = {3, 10, 10, 0, c_data};
    DpMatrix ma = {10, 10, 10, 0, ma_data};
    DpMatrix md = {10, 10, 10, 0, md_data};
    DpMatrix j = {3, 3, 3, 0, j_data};
    DpVector modal_position = {10, 0, modal_position_data};
    DpVector modal_velocity = {10, 0, modal_velocity_data};
    DpVector output = {10, 0, output_data};
    DpVector modal_velocity_output = {10, 0, modal_velocity_output_data};
    DpVec3 alpha;
    DpVec3 body_rate;
    DpVec3 angular_momentum;
    DpVec3 rhs;
    DpVec3 step_alpha;
    DpFlexDynamicsConfig config = {0};
    unsigned index;

    alpha.x = from_bits(alpha_bits[0]);
    alpha.y = from_bits(alpha_bits[1]);
    alpha.z = from_bits(alpha_bits[2]);
    if (dp_flex_modal_acceleration(&output, &c, &ma, &md, &modal_position,
                                   &modal_velocity, &alpha, NULL, NULL,
                                   NULL, NULL, NULL) != 0) {
        puts("FAIL modal acceleration returned error");
        return 1;
    }
    for (index = 0u; index < 10u; ++index) {
        if (bits(output_data[index]) != expected[index]) {
            printf("FAIL modal acceleration[%u] actual=%a expected_bits=%016llx actual_bits=%016llx\n",
                   index, output_data[index], (unsigned long long)expected[index],
                   (unsigned long long)bits(output_data[index]));
            return 1;
        }
    }
    body_rate.x = from_bits(rate_bits[0]);
    body_rate.y = from_bits(rate_bits[1]);
    body_rate.z = from_bits(rate_bits[2]);
    angular_momentum.x = from_bits(momentum_bits[0]);
    angular_momentum.y = from_bits(momentum_bits[1]);
    angular_momentum.z = from_bits(momentum_bits[2]);
    if (dp_flex_compose_rigid_rhs_coupled(&rhs, &body_rate, &angular_momentum, NULL,
                                          &c, &ma, &md, &modal_position,
                                          &modal_velocity) != 0) {
        puts("FAIL coupled rigid rhs returned error");
        return 1;
    }
    for (index = 0u; index < 3u; ++index) {
        const double actual = index == 0u ? rhs.x : (index == 1u ? rhs.y : rhs.z);
        if (bits(actual) != expected_rhs_bits[index]) {
            printf("FAIL coupled rigid rhs[%u] actual=%a expected_bits=%016llx actual_bits=%016llx\n",
                   index, actual, (unsigned long long)expected_rhs_bits[index],
                   (unsigned long long)bits(actual));
            return 1;
        }
    }
    config.rigid_matrices.coupling_3xn = c;
    config.rigid_matrices.base_inertia_3x3 = j;
    config.modal_matrix_a_nxn = &ma;
    config.modal_matrix_d_nxn = &md;
    config.angular_momentum = &angular_momentum;
    config.minus_term = NULL;
    config.rigid_map_3x3 = NULL;
    config.rigid_aux = NULL;
    memset(output_data, 0, sizeof(output_data));
    if (dp_flex_dynamics_step(&step_alpha, &output, &modal_velocity_output,
                              &body_rate, &modal_position, &modal_velocity,
                              &config) != 0) {
        puts("FAIL flex dynamics step returned error");
        return 1;
    }
    for (index = 0u; index < 3u; ++index) {
        const double actual = index == 0u ? step_alpha.x : (index == 1u ? step_alpha.y : step_alpha.z);
        if (bits(actual) != alpha_bits[index]) {
            printf("FAIL flex dynamics alpha[%u] actual=%a expected_bits=%016llx actual_bits=%016llx\n",
                   index, actual, (unsigned long long)alpha_bits[index],
                   (unsigned long long)bits(actual));
            return 1;
        }
    }
    for (index = 0u; index < 10u; ++index) {
        if (bits(output_data[index]) != expected[index] ||
            bits(modal_velocity_output_data[index]) != bits(modal_velocity_data[index])) {
            printf("FAIL flex dynamics modal[%u] accel_bits=%016llx expected=%016llx velocity_bits=%016llx expected_velocity=%016llx\n",
                   index, (unsigned long long)bits(output_data[index]),
                   (unsigned long long)expected[index],
                   (unsigned long long)bits(modal_velocity_output_data[index]),
                   (unsigned long long)bits(modal_velocity_data[index]));
            return 1;
        }
    }
    puts("flex modal acceleration, coupled RHS and dynamics step real-model original-ELF compare: PASS (bitwise)");
    return 0;
}

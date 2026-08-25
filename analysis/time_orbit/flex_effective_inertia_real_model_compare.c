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
        0x1.7e6af29ac2fc1p-12, -0x1.4103e3476b2c2p+2, -0x1.26a04d232f963p-4, -0x1.2969fff8f41d2p-1, 0x1.ff88a59a47a9ep-8, 0x1.db7533dea9c41p-3, -0x1.2a331259a76f3p-6, -0x1.cf6d43addd510p-9, -0x1.90cfd7aa5c941p-10, -0x1.12aad7a377fedp-15
    };
    double j_data[9] = {
        0x1.24a0000000000p+10, 0x1.8700000000000p+9, 0x1.7ae147ae147aep+0,
        0x1.86fffffffffffp+9, 0x1.4b9ffffffffffp+10, -0x1.1b33333333333p+4,
        0x1.7ae147ae147aep+0, -0x1.1b33333333333p+4, 0x1.02e0000000000p+11
    };
    double effective_data[9] = {0.0};
    double inverse_data[9] = {0.0};
    DpVec3 rhs;
    DpVec3 angular_acceleration;
    DpFlexRigidMatrices matrices = {
        {3, 10, 10, 0, c_data},
        {3, 3, 3, 0, j_data}
    };
    DpMatrix effective = {3, 3, 3, 0, effective_data};
    const uint64_t expected_effective_row0[3] = {
        UINT64_C(0x4091ff95dd1a7c35), UINT64_C(0x4089046a74342bd4),
        UINT64_C(0x3fd85396f5e9d7a8)
    };
    const uint64_t expected_inverse_row0[3] = {
        UINT64_C(0x3f58c8221193c21f), UINT64_C(0xbf4e624d6b582eef),
        UINT64_C(0xbedfb293a479bf7b)
    };
    const uint64_t rhs_bits[3] = {
        UINT64_C(0x3fe5ce3fcd7b4568), UINT64_C(0x3fe7722fe7e80661),
        UINT64_C(0x3fd2f6b61d53fb4f)
    };
    const uint64_t expected_acceleration[3] = {
        UINT64_C(0x3f36e0882c67b83c), UINT64_C(0x3f36dcb2526b234e),
        UINT64_C(0x3f23567451d0a876)
    };
    unsigned index;

    if (dp_flex_effective_inertia_3x3(&effective, &matrices) != 0) {
        puts("FAIL effective inertia helper returned error");
        return 1;
    }
    for (index = 0u; index < 3u; ++index) {
        if (bits(effective_data[index]) != expected_effective_row0[index]) {
            printf("FAIL effective row0[%u] actual=%a expected_bits=%016llx actual_bits=%016llx\n",
                   index, effective_data[index],
                   (unsigned long long)expected_effective_row0[index],
                   (unsigned long long)bits(effective_data[index]));
            return 1;
        }
    }
    inv_CAL_M3(effective_data, inverse_data);
    for (index = 0u; index < 3u; ++index) {
        if (bits(inverse_data[index]) != expected_inverse_row0[index]) {
            printf("FAIL inverse row0[%u] actual=%a expected_bits=%016llx actual_bits=%016llx\n",
                   index, inverse_data[index],
                   (unsigned long long)expected_inverse_row0[index],
                   (unsigned long long)bits(inverse_data[index]));
            return 1;
        }
    }
    rhs.x = from_bits(rhs_bits[0]);
    rhs.y = from_bits(rhs_bits[1]);
    rhs.z = from_bits(rhs_bits[2]);
    if (dp_flex_solve_rigid_acceleration(&angular_acceleration, &effective, &rhs) != 0) {
        puts("FAIL rigid acceleration solve returned error");
        return 1;
    }
    for (index = 0u; index < 3u; ++index) {
        const double actual = ((const double *)&angular_acceleration)[index];
        if (bits(actual) != expected_acceleration[index]) {
            printf("FAIL angular acceleration[%u] actual=%a expected_bits=%016llx actual_bits=%016llx\n",
                   index, actual, (unsigned long long)expected_acceleration[index],
                   (unsigned long long)bits(actual));
            return 1;
        }
    }
    puts("flex effective inertia real-model original-ELF compare: PASS (bitwise)");
    return 0;
}

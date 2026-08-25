#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

int main(void)
{
    static const double input[9] = {
        3.0, 1.0, -2.0,
        0.5, 4.0, 1.5,
        -1.0, 2.0, 5.0
    };
    static const uint64_t expected_input[9] = {
        UINT64_C(0x4008000000000000), UINT64_C(0x3ff0000000000000),
        UINT64_C(0xc000000000000000), UINT64_C(0x3fe0000000000000),
        UINT64_C(0x4010000000000000), UINT64_C(0x3ff8000000000000),
        UINT64_C(0xbff0000000000000), UINT64_C(0x4000000000000000),
        UINT64_C(0x4014000000000000)
    };
    static const uint64_t expected_inverse[9] = {
        UINT64_C(0x3fdd67c8a60dd67d), UINT64_C(0xbfcf22983759f22a),
        UINT64_C(0x3fd06eb3e45306eb), UINT64_C(0xbfbbacf914c1bad0),
        UINT64_C(0x3fd67c8a60dd67c9), UINT64_C(0xbfc306eb3e45306f),
        UINT64_C(0x3fc14c1bacf914c2), UINT64_C(0xbfc83759f2298376),
        UINT64_C(0x3fd3e45306eb3e45)
    };
    double inertia_data[9] = {0.0};
    double inverse_data[9] = {0.0};

    J_c_B = (DpMatrix){3, 3, 3, 0, inertia_data};
    J_c_B_inv = (DpMatrix){3, 3, 3, 0, inverse_data};
    SetInertiaTensor(input);
    if (memcmp(inertia_data, expected_input, sizeof(inertia_data)) != 0 ||
        memcmp(inverse_data, expected_inverse, sizeof(inverse_data)) != 0 ||
        J_c_B.rows != 3 || J_c_B.cols != 3 || J_c_B.row_stride != 3 ||
        J_c_B.data != inertia_data || J_c_B_inv.rows != 3 || J_c_B_inv.cols != 3 ||
        J_c_B_inv.row_stride != 3 || J_c_B_inv.data != inverse_data) {
        (void)puts("SetInertiaTensor controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: SetInertiaTensor controlled original-ELF gold compare");
    return 0;
}

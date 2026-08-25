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
    DpMatrix inertia = {3, 3, 3, 0, inertia_data};
    DpMatrix inverse = {3, 3, 3, 0, inverse_data};
    DpMatrix observed_inertia;
    DpMatrix observed_inverse;

    memset(&Sat, 0, sizeof(Sat));
    memcpy(&Sat.raw[0x08], &inertia, sizeof(inertia));
    memcpy(&Sat.raw[0x68], &inverse, sizeof(inverse));
    SetSatInertiaTensor(input);
    memcpy(&observed_inertia, &Sat.raw[0x08], sizeof(observed_inertia));
    memcpy(&observed_inverse, &Sat.raw[0x68], sizeof(observed_inverse));

    if (memcmp(inertia_data, expected_input, sizeof(inertia_data)) != 0 ||
        memcmp(inverse_data, expected_inverse, sizeof(inverse_data)) != 0 ||
        observed_inertia.rows != 3 || observed_inertia.cols != 3 ||
        observed_inertia.row_stride != 3 || observed_inertia.data != inertia_data ||
        observed_inverse.rows != 3 || observed_inverse.cols != 3 ||
        observed_inverse.row_stride != 3 || observed_inverse.data != inverse_data) {
        (void)puts("SetSatInertiaTensor controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: SetSatInertiaTensor controlled original-ELF gold compare");
    return 0;
}

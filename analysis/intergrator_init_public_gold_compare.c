#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

static const uint64_t expected_t_bits = UINT64_C(0x40934a0000000000);
static const uint64_t expected_y_bits[DP_STATE_DIM] = {
    UINT64_C(0x3fc0000000000000), UINT64_C(0xbfe0000000000000),
    UINT64_C(0x4002000000000000), UINT64_C(0xc00e000000000000),
    UINT64_C(0xbff4000000000000), UINT64_C(0),
    UINT64_C(0x3ff8000000000000), UINT64_C(0x415ab3f000000000),
    UINT64_C(0xc029000000000000), UINT64_C(0x4045200000000000),
    UINT64_C(0xc0bd7a0000000000), UINT64_C(0x3fb0000000000000),
    UINT64_C(0x4020000000000000), UINT64_C(0), UINT64_C(0), UINT64_C(0),
    UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0),
    UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0),
    UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0),
    UINT64_C(0), UINT64_C(0)
};

int main(void)
{
    double quaternion_xyz[3] = {-0.5, 2.25, -3.75};
    double rate_data[3] = {-1.25, 0.0, 1.5};
    double position_data[3] = {7000000.0, -12.5, 42.25};
    double velocity_data[3] = {-7546.0, 0.0625, 8.0};
    DpQuatAbi attitude = {0.125, {3, 0, quaternion_xyz}};
    DpVector body_rate = {3, 0, rate_data};
    uint64_t actual_bits;
    size_t index;

    memset(y, 0xa5, sizeof(y));
    t = -999.0;
    intergrator_init(&attitude, &body_rate, position_data, velocity_data, 1234.5);
    memcpy(&actual_bits, &t, sizeof(actual_bits));
    if (actual_bits != expected_t_bits) {
        (void)printf("intergrator_init t mismatch: got=%#llx expected=%#llx\n",
                     (unsigned long long)actual_bits, (unsigned long long)expected_t_bits);
        return 1;
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        memcpy(&actual_bits, &y[index], sizeof(actual_bits));
        if (actual_bits != expected_y_bits[index]) {
            (void)printf("intergrator_init y[%zu] mismatch: got=%#llx expected=%#llx\n", index,
                         (unsigned long long)actual_bits,
                         (unsigned long long)expected_y_bits[index]);
            return 1;
        }
    }
    (void)puts("PASS: intergrator_init controlled original-ELF gold compare");
    return 0;
}

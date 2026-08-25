#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

int main(void)
{
    DpInitialConditions initial;
    static const uint64_t expected_y[13] = {
        UINT64_C(0x3ff0000000000000), UINT64_C(0x0000000000000000),
        UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000),
        UINT64_C(0x3f847ae140000000), UINT64_C(0xbf947ae140000000),
        UINT64_C(0x3f9eb851e0000000), UINT64_C(0x412844847cff09ca),
        UINT64_C(0x41598b6d561f6e7b), UINT64_C(0x413894fdcddd9221),
        UINT64_C(0xc0bb0157ded6a3d2), UINT64_C(0x4052a87f4bf2dee0),
        UINT64_C(0x40a90027857bdf2b)
    };
    static const uint64_t expected_j[9] = {
        UINT64_C(0x4008000000000000), UINT64_C(0x3ff0000000000000),
        UINT64_C(0xc000000000000000), UINT64_C(0x3fe0000000000000),
        UINT64_C(0x4010000000000000), UINT64_C(0x3ff8000000000000),
        UINT64_C(0xbff0000000000000), UINT64_C(0x4000000000000000),
        UINT64_C(0x4014000000000000)
    };
    static const uint64_t expected_jinv[9] = {
        UINT64_C(0x3fdd67c8a60dd67d), UINT64_C(0xbfcf22983759f22a),
        UINT64_C(0x3fd06eb3e45306eb), UINT64_C(0xbfbbacf914c1bad0),
        UINT64_C(0x3fd67c8a60dd67c9), UINT64_C(0xbfc306eb3e45306f),
        UINT64_C(0x3fc14c1bacf914c2), UINT64_C(0xbfc83759f2298376),
        UINT64_C(0x3fd3e45306eb3e45)
    };
    uint64_t mass_bits;

    memset(&initial, 0, sizeof(initial));
    initial.step_time = 0.1;
    initial.initial_time_or_epoch = 1.0;
    initial.initial_attitude_vector = (DpVec3){0.0, 0.0, 0.0};
    initial.initial_angular_rate_f32[0] = 0.01f;
    initial.initial_angular_rate_f32[1] = -0.02f;
    initial.initial_angular_rate_f32[2] = 0.03f;
    initial.time_parameters[0] = 2024.0;
    initial.time_parameters[1] = 2.0;
    initial.time_parameters[2] = 29.0;
    initial.time_parameters[3] = 23.0;
    initial.time_parameters[4] = 59.0;
    initial.time_parameters[5] = 59.5;
    initial.orbit_elements[0] = 7000000.0;
    initial.orbit_elements[1] = 0.01;
    initial.orbit_elements[2] = 0.5;
    initial.orbit_elements[3] = 1.0;
    initial.orbit_elements[4] = 0.2;
    initial.orbit_elements[5] = 0.3;
    initial.inertia_tensor[0] = 3.0;
    initial.inertia_tensor[1] = 1.0;
    initial.inertia_tensor[2] = -2.0;
    initial.inertia_tensor[3] = 0.5;
    initial.inertia_tensor[4] = 4.0;
    initial.inertia_tensor[5] = 1.5;
    initial.inertia_tensor[6] = -1.0;
    initial.inertia_tensor[7] = 2.0;
    initial.inertia_tensor[8] = 5.0;
    initial.spacecraft_mass = 600.0;

    dyn_init(&initial);
    memcpy(&mass_bits, &SpacecraftMass, sizeof(mass_bits));
    if (memcmp(y, expected_y, sizeof(expected_y)) != 0 ||
        memcmp(J_c_B.data, expected_j, sizeof(expected_j)) != 0 ||
        memcmp(J_c_B_inv.data, expected_jinv, sizeof(expected_jinv)) != 0 ||
        mass_bits != UINT64_C(0x4082c00000000000)) {
        (void)puts("dyn_init controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: dyn_init controlled original-ELF gold compare");
    return 0;
}

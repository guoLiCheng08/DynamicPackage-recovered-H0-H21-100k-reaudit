#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

int main(void)
{
    double position[3] = {0.0, 0.0, 0.0};
    double velocity[3] = {0.0, 0.0, 0.0};
    static const uint64_t expected_position[3] = {
        UINT64_C(0x3ff4000000000000),
        UINT64_C(0xc004000000000000),
        UINT64_C(0x400e000000000000)
    };
    static const uint64_t expected_velocity[3] = {
        UINT64_C(0xc010800000000000),
        UINT64_C(0x4016000000000000),
        UINT64_C(0xc01a800000000000)
    };

    y[7] = 1.25;
    y[8] = -2.5;
    y[9] = 3.75;
    y[10] = -4.125;
    y[11] = 5.5;
    y[12] = -6.625;
    Get_Orbit_RV(position, velocity);
    if (memcmp(position, expected_position, sizeof(position)) != 0 ||
        memcmp(velocity, expected_velocity, sizeof(velocity)) != 0) {
        (void)puts("Get_Orbit_RV controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: Get_Orbit_RV controlled original-ELF gold compare");
    return 0;
}

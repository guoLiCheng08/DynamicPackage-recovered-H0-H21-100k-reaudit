#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"
#include "dynamic_time.h"

static uint64_t dp_bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    static const uint64_t expected[3] = {
        UINT64_C(0x3f8a7b843cbc2919),
        UINT64_C(0xbfb1ee4c33ca1129),
        UINT64_C(0xbf9ee56f9c30da9d)
    };
    const DpCalendarTime calendar = {2031.0, 12.0, 31.0, 23.0, 59.0, 50.0};
    double gci_data[3] = {-12000000.0, 65000000.0, 28000000.0};
    double ecef_data[3] = {0.0, 0.0, 0.0};
    double ecef_accel_data[3] = {0.0, 0.0, 0.0};
    double gci_accel_data[3] = {0.0, 0.0, 0.0};
    DpVector gci = {3, 0, gci_data};
    DpVector ecef = {3, 0, ecef_data};
    DpVector ecef_accel = {3, 0, ecef_accel_data};
    DpVector gci_accel = {3, 0, gci_accel_data};
    DpVec3 position;
    DpVec3 position_dot;
    DpVec3 acceleration;
    DpVec3 zero_velocity = {0.0, 0.0, 0.0};
    DpOrbitGravityModel model = DP_ORBIT_EARTH_MODEL;
    unsigned index;

    GCI2ECEF(&ecef, &gci, &calendar);
    position.x = ecef_data[0];
    position.y = ecef_data[1];
    position.z = ecef_data[2];
    model.j2 = 0.0;
    model.j3 = 0.0;
    model.j4 = 0.0;
    dp_orbit_dynamic_zonal(&position_dot, &acceleration, &position, &zero_velocity,
                           NULL, &model);
    ecef_accel_data[0] = acceleration.x;
    ecef_accel_data[1] = acceleration.y;
    ecef_accel_data[2] = acceleration.z;
    ECEF2GCI(&gci_accel, &ecef_accel, &calendar);
    for (index = 0u; index < 3u; ++index) {
        if (dp_bits(gci_accel_data[index]) != expected[index]) {
            fprintf(stderr, "no-zonal field=%u actual=%016llx expected=%016llx\n", index,
                    (unsigned long long)dp_bits(gci_accel_data[index]),
                    (unsigned long long)expected[index]);
            return 1;
        }
    }
    puts("orbit_dynamic high-ecc shadow no-zonal original-ELF compare: PASS (bitwise)");
    return 0;
}

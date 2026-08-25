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
    const DpCalendarTime calendar = {2031.0, 12.0, 31.0, 23.0, 59.0, 50.0};
    double gci_data[3] = {-12000000.0, 65000000.0, 28000000.0};
    double ecef_data[3] = {0.0, 0.0, 0.0};
    double ecef_accel_data[3] = {0.0, 0.0, 0.0};
    DpVector gci = {3, 0, gci_data};
    DpVector ecef = {3, 0, ecef_data};
    DpVec3 position;
    DpVec3 position_dot;
    DpVec3 acceleration;
    DpVec3 zero_velocity = {0.0, 0.0, 0.0};
    DpOrbitGravityModel model = DP_ORBIT_EARTH_MODEL;

    GCI2ECEF(&ecef, &gci, &calendar);
    position.x = ecef_data[0];
    position.y = ecef_data[1];
    position.z = ecef_data[2];
    model.spacecraft_mass = 1000.0;
    dp_orbit_dynamic_zonal(&position_dot, &acceleration, &position, &zero_velocity,
                           NULL, &model);
    ecef_accel_data[0] = acceleration.x;
    ecef_accel_data[1] = acceleration.y;
    ecef_accel_data[2] = acceleration.z;
    printf("ecef_accel_bits=(%016llx,%016llx,%016llx)\n",
           (unsigned long long)dp_bits(ecef_accel_data[0]),
           (unsigned long long)dp_bits(ecef_accel_data[1]),
           (unsigned long long)dp_bits(ecef_accel_data[2]));
    return 0;
}

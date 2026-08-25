#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    const DpVec3 position = {7000000.0, -1210000.0, 2300000.0};
    const DpVec3 velocity = {0.0, 0.0, 0.0};
    const DpVec3 zero_force = {0.0, 0.0, 0.0};
    DpOrbitGravityModel model = DP_ORBIT_EARTH_MODEL;
    DpVec3 position_dot;
    DpVec3 acceleration;

    model.spacecraft_mass = 100.0;
    dp_orbit_dynamic_zonal(&position_dot, &acceleration, &position, &velocity,
                           &zero_force, &model);
    printf("recovered acceleration=(%.17g, %.17g, %.17g) bits=(%016llx, %016llx, %016llx)\n",
           acceleration.x, acceleration.y, acceleration.z,
           (unsigned long long)bits(acceleration.x),
           (unsigned long long)bits(acceleration.y),
           (unsigned long long)bits(acceleration.z));
    return 0;
}

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"
#include "dynamic_time.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    double gci_raw[3] = {7000000.0, -1210000.0, 2300000.0};
    double ecef_raw[3] = {0.0, 0.0, 0.0};
    double ecef_acc_raw[3] = {0.0, 0.0, 0.0};
    double gci_acc_raw[3] = {0.0, 0.0, 0.0};
    DpVector gci = {3, 0, gci_raw};
    DpVector ecef = {3, 0, ecef_raw};
    DpVector ecef_acc = {3, 0, ecef_acc_raw};
    DpVector gci_acc = {3, 0, gci_acc_raw};
    DpCalendarTime calendar;
    DpVec3 position;
    DpVec3 velocity = {0.0, 0.0, 0.0};
    DpVec3 zero_force = {0.0, 0.0, 0.0};
    DpVec3 position_dot;
    DpVec3 acceleration;
    DpOrbitGravityModel model = DP_ORBIT_EARTH_MODEL;

    TimeInit(2020.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    TimeArrayGet((double *)&calendar);
    GCI2ECEF(&ecef, &gci, &calendar);
    position.x = ecef_raw[0];
    position.y = ecef_raw[1];
    position.z = ecef_raw[2];
    model.j5 = 0.0;
    model.j6 = 0.0;
    model.spacecraft_mass = 100.0;
    dp_orbit_dynamic_zonal(&position_dot, &acceleration, &position, &velocity,
                           &zero_force, &model);
    ecef_acc_raw[0] = acceleration.x;
    ecef_acc_raw[1] = acceleration.y;
    ecef_acc_raw[2] = acceleration.z;
    ECEF2GCI(&gci_acc, &ecef_acc, &calendar);
    printf("ecef=(%.17g, %.17g, %.17g)\n", ecef_raw[0], ecef_raw[1], ecef_raw[2]);
    printf("ecef generic J2-J4=(%.17g, %.17g, %.17g)\n",
           ecef_acc_raw[0], ecef_acc_raw[1], ecef_acc_raw[2]);
    printf("gci generic J2-J4=(%.17g, %.17g, %.17g) bits=(%016llx, %016llx, %016llx)\n",
           gci_acc_raw[0], gci_acc_raw[1], gci_acc_raw[2],
           (unsigned long long)bits(gci_acc_raw[0]),
           (unsigned long long)bits(gci_acc_raw[1]),
           (unsigned long long)bits(gci_acc_raw[2]));
    return 0;
}

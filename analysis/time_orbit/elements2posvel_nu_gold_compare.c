#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"

typedef struct {
    double semi_major_axis;
    double eccentricity;
    double inclination;
    double raan;
    double argument_of_periapsis;
    double true_anomaly;
    double expected_position[3];
    double expected_velocity[3];
} ElementsNuCase;

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    /* 原 ELF Elements2PosVel_nu：隔离 GDB 直接调用所得金标。 */
    const ElementsNuCase cases[] = {
        {7000000.0, 0.1, 0.7, 1.3, 0.45, 2.2616708994930574,
         {-4073306.8929038076, -5851722.1589736249, 1987412.5991544293},
         {3643.5044963902146, -4634.4388877243546, -4001.2366700306461}},
        {42164000.0, 0.65, 0.2, -0.8, 2.4, 3.5920276336741965,
         {27327503.580929443, -51840348.836246923, -3347546.9801235138},
         {932.97950821071765, 1763.3612138445376, 384.70799507620728}},
    };
    unsigned case_index;

    for (case_index = 0u; case_index < sizeof(cases) / sizeof(cases[0]);
         ++case_index) {
        const ElementsNuCase *const c = &cases[case_index];
        double position[3] = {0.0};
        double velocity[3] = {0.0};
        unsigned element;
        Elements2PosVel_nu(position, velocity, c->semi_major_axis,
                           c->eccentricity, c->inclination, c->raan,
                           c->argument_of_periapsis, c->true_anomaly);
        for (element = 0u; element < 3u; ++element) {
            if (bits(position[element]) != bits(c->expected_position[element])) {
                printf("FAIL position case=%u element=%u actual=%a expected=%a "
                       "actual_bits=%016llx expected_bits=%016llx\n",
                       case_index, element, position[element],
                       c->expected_position[element],
                       (unsigned long long)bits(position[element]),
                       (unsigned long long)bits(c->expected_position[element]));
                return 1;
            }
            if (bits(velocity[element]) != bits(c->expected_velocity[element])) {
                printf("FAIL velocity case=%u element=%u actual=%a expected=%a "
                       "actual_bits=%016llx expected_bits=%016llx\n",
                       case_index, element, velocity[element],
                       c->expected_velocity[element],
                       (unsigned long long)bits(velocity[element]),
                       (unsigned long long)bits(c->expected_velocity[element]));
                return 1;
            }
        }
    }
    puts("Elements2PosVel_nu original-ELF gold compare: PASS (bitwise)");
    return 0;
}

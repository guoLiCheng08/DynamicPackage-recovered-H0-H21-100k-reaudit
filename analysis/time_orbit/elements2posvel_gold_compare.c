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
    double mean_anomaly;
    double expected_position[3];
    double expected_velocity[3];
} ElementsCase;

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    /* 原 ELF Elements2PosVel_M：隔离 GDB 直接调用所得金标。 */
    const ElementsCase cases[] = {
        {7000000.0, 0.1, 0.7, 1.3, 0.45, 2.1,
         {-4073306.892903815, -5851722.1589736138, 1987412.5991544391},
         {3643.5044963902051, -4634.4388877243673, -4001.236670030642}},
        {42164000.0, 0.65, 0.2, -0.8, 2.4, -1.7,
         {27327503.58092948, -51840348.8362469, -3347546.980123505},
         {932.97950821071584, 1763.3612138445401, 384.70799507620745}},
    };
    unsigned case_index;

    for (case_index = 0u; case_index < sizeof(cases) / sizeof(cases[0]);
         ++case_index) {
        double position[3] = {0.0};
        double velocity[3] = {0.0};
        unsigned element;
        const ElementsCase *const c = &cases[case_index];
        Elements2PosVel_M(position, velocity, c->semi_major_axis,
                          c->eccentricity, c->inclination, c->raan,
                          c->argument_of_periapsis, c->mean_anomaly);
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
    puts("Elements2PosVel_M original-ELF gold compare: PASS (bitwise)");
    return 0;
}

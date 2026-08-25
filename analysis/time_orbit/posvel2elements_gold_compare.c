#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"

typedef struct {
    double position[3];
    double velocity[3];
    double expected[8];
} PosVelCase;

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    /* 原 ELF PosVel2Elements_M：隔离 GDB 直接调用所得金标。 */
    const PosVelCase cases[] = {
        {{-4073306.892903815, -5851722.1589736138, 1987412.5991544391},
         {3643.5044963902051, -4634.4388877243673, -4001.236670030642},
         {6999999.9999999972, 0.10000000000000009, 0.69999999999999973,
          1.3, 0.44999999999999729, 2.2616708994930574,
          2.1000000000000028, 2.711670899493055}},
        {{27327503.58092948, -51840348.8362469, -3347546.980123505},
         {932.97950821071584, 1763.3612138445401, 384.70799507620745},
         {42164000.0, 0.65000000000000002, 0.20000000000000115,
          5.4831853071795864, 2.3999999999999999, 3.5920276336741965,
          4.5831853071795861, 5.992027633674196}},
    };
    unsigned case_index;

    for (case_index = 0u; case_index < sizeof(cases) / sizeof(cases[0]);
         ++case_index) {
        double actual[8] = {0.0};
        unsigned field;
        PosVel2Elements_M(&actual[0], &actual[1], &actual[2], &actual[3],
                          &actual[4], &actual[5], &actual[6], &actual[7],
                          cases[case_index].position, cases[case_index].velocity);
        for (field = 0u; field < 8u; ++field) {
            if (bits(actual[field]) != bits(cases[case_index].expected[field])) {
                printf("FAIL case=%u field=%u actual=%a expected=%a "
                       "actual_bits=%016llx expected_bits=%016llx\n",
                       case_index, field, actual[field],
                       cases[case_index].expected[field],
                       (unsigned long long)bits(actual[field]),
                       (unsigned long long)bits(cases[case_index].expected[field]));
                return 1;
            }
        }
    }
    puts("PosVel2Elements_M original-ELF gold compare: PASS (bitwise)");
    return 0;
}

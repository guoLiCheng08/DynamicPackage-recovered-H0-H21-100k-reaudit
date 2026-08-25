#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"

typedef struct {
    double position[3];
    double velocity[3];
    double expected[8];
} DegenerateCase;

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    const DegenerateCase cases[] = {
        {{-4431241.4282810194, -4799941.4308603881, 2514888.0023961156},
         {3490.3874920223357, -5336.4557452363915, -4035.1400779840646},
         {6999999.9999999991, 2.4705944203252814e-16, 0.69999999999999984,
          1.3, 0.0, 2.5500000000000003, 2.5500000000000003,
          2.5500000000000003}},
        {{-6058112.4461465934, 4070232.2512063403, 0.0},
         {-4559.3926008976832, -5612.2779225600771, -0.0},
         {7000000.0, 0.10000000000000009, 0.0, 0.0,
          0.44999999999999962, 2.1000000000000005,
          1.9208188455345012, 2.5500000000000003}},
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
    puts("PosVel2Elements_M degenerate original-ELF gold compare: PASS (bitwise)");
    return 0;
}

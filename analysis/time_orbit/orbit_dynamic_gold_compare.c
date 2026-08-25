#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"
#include "dynamic_time.h"

typedef struct {
    double calendar[6];
    double position[3];
    double force[3];
    double mass;
    uint64_t expected[3];
} OrbitDynamicCase;

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    const OrbitDynamicCase cases[] = {
        {{2020.0, 1.0, 2.0, 3.0, 4.0, 5.0},
         {7000000.0, -1210000.0, 2300000.0},
         {0.0, 0.0, 0.0}, 100.0,
         {UINT64_C(0xc01ad4f134716921), UINT64_C(0x3ff28d5f845a3333),
          UINT64_C(0xc001b09e7aacd8aa)}},
        {{2024.0, 6.0, 15.0, 12.0, 34.0, 56.0},
         {-4431241.4282810194, -4799941.4308603881, 2514888.0023961156},
         {22.0, -16.0, 7.0}, 450.0,
         {UINT64_C(0x4014ce499c6c441e), UINT64_C(0x40162ed917b0b75e),
          UINT64_C(0xc007586af110b4a3)}},
        {{2030.0, 12.0, 31.0, 23.0, 59.0, 30.0},
         {7100000.0, -100.0, 10.0},
         {0.0, 0.0, 0.0}, 100.0,
         {UINT64_C(0xc01fae1244ef896c), UINT64_C(0x3f1d3e02f995b3ee),
          UINT64_C(0xbf1e604db597bf00)}}
    };
    unsigned case_index;

    for (case_index = 0u; case_index < sizeof(cases) / sizeof(cases[0]);
         ++case_index) {
        double actual_raw[3] = {0.0, 0.0, 0.0};
        DpVector position = {3, 0, (double *)cases[case_index].position};
        DpVector actual = {3, 0, actual_raw};
        unsigned field;

        TimeInit(cases[case_index].calendar[0], cases[case_index].calendar[1],
                 cases[case_index].calendar[2], cases[case_index].calendar[3],
                 cases[case_index].calendar[4], cases[case_index].calendar[5]);
        SpacecraftMass = cases[case_index].mass;
        F_I_external.data[0] = cases[case_index].force[0];
        F_I_external.data[1] = cases[case_index].force[1];
        F_I_external.data[2] = cases[case_index].force[2];
        orbit_dynamic(&actual, &position);
        for (field = 0u; field < 3u; ++field) {
            if (bits(actual_raw[field]) != cases[case_index].expected[field]) {
                printf("FAIL case=%u field=%u actual=%a expected_bits=%016llx actual_bits=%016llx\n",
                       case_index, field, actual_raw[field],
                       (unsigned long long)cases[case_index].expected[field],
                       (unsigned long long)bits(actual_raw[field]));
                return 1;
            }
        }
    }
    puts("orbit_dynamic original-ELF gold compare: PASS (bitwise)");
    return 0;
}

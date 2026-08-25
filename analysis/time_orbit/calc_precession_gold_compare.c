#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_time.h"

typedef struct {
    double centuries;
    double expected[9];
} PrecessionCase;

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    /* 原 ELF Calc_Precession：隔离 GDB 直接调用所得金标。 */
    const PrecessionCase cases[] = {
        {0.0, {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}},
        {0.2424, {
            0.99998253331484932, -0.005420847846753612,
            -0.0023553075889041486, 0.0054208478461271418,
            0.99998530707599453, -6.3842037703410626e-06,
            0.002355307590345997, -6.3836718108140973e-06,
            0.9999972262388549,
        }},
        {-0.5, {
            0.99992570856960161, 0.011178891768168123,
            0.0048589834755412798, -0.011178891779510376,
            0.99993751386826224, -2.7157699809415468e-05,
            -0.004858983449446558, -2.7162368206454864e-05,
            0.99998819470133937,
        }},
    };
    unsigned case_index;

    for (case_index = 0u; case_index < sizeof(cases) / sizeof(cases[0]);
         ++case_index) {
        double actual[9] = {0.0};
        DpMatrix matrix = {3, 3, 3, 0, actual};
        unsigned index;
        Calc_Precession(cases[case_index].centuries, &matrix);
        for (index = 0u; index < 9u; ++index) {
            if (bits(actual[index]) != bits(cases[case_index].expected[index])) {
                printf("FAIL case=%u element=%u actual=%a expected=%a "
                       "actual_bits=%016llx expected_bits=%016llx\n",
                       case_index, index, actual[index],
                       cases[case_index].expected[index],
                       (unsigned long long)bits(actual[index]),
                       (unsigned long long)bits(cases[case_index].expected[index]));
                return 1;
            }
        }
    }
    puts("Calc_Precession original-ELF gold compare: PASS (bitwise)");
    return 0;
}

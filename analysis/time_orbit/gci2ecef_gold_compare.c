#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_time.h"

typedef struct {
    double input[3];
    DpCalendarTime time;
    double expected[3];
} GCI2ECEFCase;

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    /* 原 ELF GCI2ECEF：隔离 GDB 直接调用所得金标。 */
    const GCI2ECEFCase cases[] = {
        {{7000000.0, -1200000.0, 2500000.0},
         {2024.0, 2.0, 29.0, 6.0, 30.0, 15.5},
         {-548936.84341142606, 7075042.6046172278, 2516434.0811546384}},
        {{-1000.25, 4000.5, 5500.75},
         {1985.0, 7.0, 1.0, 0.0, 0.0, 0.125},
         {-4107.6138144978613, -341.55300932089494, 5502.1451423758517}},
    };
    unsigned case_index;

    for (case_index = 0u; case_index < sizeof(cases) / sizeof(cases[0]);
         ++case_index) {
        double actual[3] = {0.0};
        DpVector input = {3, 0, (double *)cases[case_index].input};
        DpVector output = {3, 0, actual};
        unsigned element;
        GCI2ECEF(&output, &input, &cases[case_index].time);
        for (element = 0u; element < 3u; ++element) {
            if (bits(actual[element]) != bits(cases[case_index].expected[element])) {
                printf("FAIL case=%u element=%u actual=%a expected=%a "
                       "actual_bits=%016llx expected_bits=%016llx\n",
                       case_index, element, actual[element],
                       cases[case_index].expected[element],
                       (unsigned long long)bits(actual[element]),
                       (unsigned long long)bits(cases[case_index].expected[element]));
                return 1;
            }
        }
    }
    puts("GCI2ECEF original-ELF gold compare: PASS (bitwise)");
    return 0;
}

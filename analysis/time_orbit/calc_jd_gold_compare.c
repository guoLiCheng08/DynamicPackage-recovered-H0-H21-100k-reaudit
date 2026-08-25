#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_time.h"

typedef struct {
    double year;
    double month;
    double day;
    double hour;
    double minute;
    double second;
    double expected;
} CalcJDCase;

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    /* 由隔离 GDB 在原 ELF Calc_JD 符号上直接调用并记录。 */
    const CalcJDCase cases[] = {
        {2000.0, 1.0, 1.0, 12.0, 0.0, 0.0, 2451545.0},
        {2024.0, 2.0, 29.0, 6.0, 30.0, 15.5, 2460369.7710127314},
        {1999.0, 12.0, 31.0, 23.0, 59.0, 59.25, 2451544.4999913196},
        {1985.0, 7.0, 1.0, 0.0, 0.0, 0.125, 2446247.5000014468},
    };
    unsigned i;
    for (i = 0u; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const CalcJDCase *const c = &cases[i];
        const double actual = Calc_JD(c->year, c->month, c->day, c->hour,
                                      c->minute, c->second);
        if (bits(actual) != bits(c->expected)) {
            printf("FAIL case=%u actual=%a expected=%a actual_bits=%016llx expected_bits=%016llx\n",
                   i, actual, c->expected,
                   (unsigned long long)bits(actual),
                   (unsigned long long)bits(c->expected));
            return 1;
        }
    }
    puts("Calc_JD original-ELF gold compare: PASS (bitwise)");
    return 0;
}

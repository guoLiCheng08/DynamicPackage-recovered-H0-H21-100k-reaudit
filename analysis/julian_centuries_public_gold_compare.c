#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_time.h"

static int compare_bits(double actual, uint64_t expected, const char *label)
{
    uint64_t actual_bits;

    memcpy(&actual_bits, &actual, sizeof(actual_bits));
    if (actual_bits != expected) {
        (void)printf("%s actual=%#llx expected=%#llx\n", label,
                     (unsigned long long)actual_bits, (unsigned long long)expected);
        return -1;
    }
    return 0;
}

int main(void)
{
    const double j2000 = JulianCenturies(2000.0, 1.0, 1.0, 12.0, 0.0, 0.0);
    const double leap_day = JulianCenturies(2024.0, 2.0, 29.0, 23.0, 59.0, 59.5);

    if (compare_bits(j2000, UINT64_C(0x0000000000000000), "J2000") != 0 ||
        compare_bits(leap_day, UINT64_C(0x3fceedb3239bbea6), "leap-day") != 0) {
        return 1;
    }
    (void)puts("PASS: JulianCenturies controlled original-ELF gold compare");
    return 0;
}

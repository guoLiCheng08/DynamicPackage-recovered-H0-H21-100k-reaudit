#include <stdio.h>
#include <string.h>

#include "dynamic_environment.h"
#include "dynamic_math.h"
#include "dynamic_time.h"

int main(void)
{
    FILE *file;
    double expected[3];
    double actual[3] = {0.0, 0.0, 0.0};
    DpVector out = {3, 0, actual};
    const double jd = Calc_JD(2031.0, 12.0, 31.0, 23.0, 59.0, 50.0);

    file = fopen("analysis/time_orbit/gold_orbit_time_boundary_sun_gci.bin", "rb");
    if (file == NULL || fread(expected, 1u, sizeof(expected), file) != sizeof(expected) || fclose(file) != 0) {
        return 1;
    }
    dp_sun_vector(jd, &out);
    if (memcmp(actual, expected, sizeof(actual)) != 0) {
        unsigned index;
        for (index = 0u; index < 3u; ++index) {
            printf("sun[%u] actual=%.17g expected=%.17g\n", index, actual[index], expected[index]);
        }
        return 1;
    }
    puts("GetSunVector orbit-time original-ELF compare: PASS (bitwise)");
    return 0;
}

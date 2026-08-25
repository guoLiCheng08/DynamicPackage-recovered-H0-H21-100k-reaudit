#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"

typedef struct {
    double eccentric_anomaly;
    double eccentricity;
    double sine_eccentric_anomaly;
    double expected;
} Eccentric2MeanCase;

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    const Eccentric2MeanCase cases[] = {
        {2.181001813890183, 0.1, 0.81901443222271053, 2.0991003706679119},
        {-1.945363558764963, 0.65, -0.93088527760797902, -1.3402881283197765},
        {0.0, 0.9, 0.0, 0.0},
    };
    unsigned index;

    for (index = 0u; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        const Eccentric2MeanCase *const c = &cases[index];
        const double actual = Eccentric2Mean(c->eccentric_anomaly,
                                             c->eccentricity,
                                             c->sine_eccentric_anomaly);
        if (bits(actual) != bits(c->expected)) {
            printf("FAIL case=%u actual=%a expected=%a actual_bits=%016llx "
                   "expected_bits=%016llx\n", index, actual, c->expected,
                   (unsigned long long)bits(actual),
                   (unsigned long long)bits(c->expected));
            return 1;
        }
    }
    puts("Eccentric2Mean original-ELF gold compare: PASS (bitwise)");
    return 0;
}

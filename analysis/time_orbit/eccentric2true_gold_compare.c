#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"

typedef struct {
    double eccentric_anomaly;
    double eccentricity;
    double cosine_eccentric_anomaly;
    double sine_eccentric_anomaly;
    double sqrt_one_minus_eccentricity_squared;
    double expected;
} Eccentric2TrueCase;

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    const Eccentric2TrueCase cases[] = {
        {2.181001813890183, 0.1, -0.57377291658888108, 0.81901443222271053,
         0.99498743710662, 2.2607712701609666},
        {-1.945363558764963, 0.65, -0.36594409472331852,
         -0.93063682110172078, 0.75993420767853315, -2.5333931425567835},
        {0.0, 0.0, 1.0, 0.0, 1.0, 0.0},
    };
    unsigned index;

    for (index = 0u; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        const Eccentric2TrueCase *const c = &cases[index];
        const double actual = Eccentric2True(
            c->eccentric_anomaly, c->eccentricity,
            c->cosine_eccentric_anomaly, c->sine_eccentric_anomaly,
            c->sqrt_one_minus_eccentricity_squared);
        if (bits(actual) != bits(c->expected)) {
            printf("FAIL case=%u actual=%a expected=%a actual_bits=%016llx "
                   "expected_bits=%016llx\n", index, actual, c->expected,
                   (unsigned long long)bits(actual),
                   (unsigned long long)bits(c->expected));
            return 1;
        }
    }
    puts("Eccentric2True original-ELF gold compare: PASS (bitwise)");
    return 0;
}

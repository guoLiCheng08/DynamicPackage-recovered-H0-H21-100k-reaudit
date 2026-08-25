#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"
#include "dynamic_orbit.h"
#include "dynamic_time.h"

typedef struct DpCase {
    const char *name;
    DpCalendarTime calendar;
    double gci[3];
} DpCase;

static uint64_t dp_bits(double value)
{
    uint64_t bits;
    memcpy(&bits, &value, sizeof(bits));
    return bits;
}

int main(void)
{
    static const DpCase cases[] = {
        {"case0-default-orbit-gold", {2020.0, 1.0, 2.0, 3.0, 4.0, 5.0},
         {7000000.0, -1210000.0, 2300000.0}},
        {"case1-orbit-gold", {2024.0, 6.0, 15.0, 12.0, 34.0, 56.0},
         {-4431241.4282810194, -4799941.4308603881, 2514888.0023961156}},
        {"case2-near-equatorial-gold", {2030.0, 12.0, 31.0, 23.0, 59.0, 30.0},
         {7100000.0, -100.0, 10.0}},
        {"high-ecc-shadow", {2031.0, 12.0, 31.0, 23.0, 59.0, 50.0},
         {-12000000.0, 65000000.0, 28000000.0}}
    };
    size_t index;

    for (index = 0u; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        double input[3];
        double ecef_data[3] = {0.0, 0.0, 0.0};
        double sum;
        double radius;
        double product_from_sum;
        double product_from_radius;
        DpVector gci = {3, 0, input};
        DpVector ecef = {3, 0, ecef_data};

        memcpy(input, cases[index].gci, sizeof(input));
        GCI2ECEF(&ecef, &gci, &cases[index].calendar);
        sum = ecef_data[0] * ecef_data[0] + ecef_data[1] * ecef_data[1] +
              ecef_data[2] * ecef_data[2];
        radius = vector_nrm2(&ecef);
        product_from_sum = radius * sum;
        product_from_radius = radius * radius;
        product_from_radius *= radius;
        printf("%s sum=%016llx radius=%016llx rsum=%016llx rrr=%016llx %s\n",
               cases[index].name, (unsigned long long)dp_bits(sum),
               (unsigned long long)dp_bits(radius),
               (unsigned long long)dp_bits(product_from_sum),
               (unsigned long long)dp_bits(product_from_radius),
               dp_bits(product_from_sum) == dp_bits(product_from_radius) ? "same" : "different");
    }
    return 0;
}

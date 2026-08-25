#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"
#include "dynamic_time.h"

#define GOLD_PATH "analysis/time_orbit/gold_orbit_shadow_ecef_position.bin"

static uint64_t dp_bits(double value)
{
    uint64_t bits;
    memcpy(&bits, &value, sizeof(bits));
    return bits;
}

static int read_exact(void *out, size_t bytes)
{
    FILE *file = fopen(GOLD_PATH, "rb");
    size_t actual;

    if (file == NULL) {
        perror(GOLD_PATH);
        return -1;
    }
    actual = fread(out, 1u, bytes, file);
    if (fclose(file) != 0 || actual != bytes) {
        return -1;
    }
    return 0;
}

int main(void)
{
    const DpCalendarTime calendar = {2031.0, 12.0, 31.0, 23.0, 59.0, 50.0};
    double gci_data[3] = {-12000000.0, 65000000.0, 28000000.0};
    double ecef_data[3] = {0.0, 0.0, 0.0};
    double expected_ecef[3];
    double sum;
    double radius;
    double r_times_sum;
    double r_cubed;
    double central_sum;
    double central_cubed;
    unsigned index;
    DpVector gci = {3, 0, gci_data};
    DpVector ecef = {3, 0, ecef_data};

    if (read_exact(expected_ecef, sizeof(expected_ecef)) != 0) {
        return 1;
    }
    GCI2ECEF(&ecef, &gci, &calendar);
    sum = ecef_data[0] * ecef_data[0] + ecef_data[1] * ecef_data[1] +
          ecef_data[2] * ecef_data[2];
    radius = vector_nrm2(&ecef);
    r_times_sum = radius * sum;
    r_cubed = radius * radius;
    r_cubed *= radius;
    central_sum = -DP_ORBIT_EARTH_MODEL.gravitational_parameter / r_times_sum;
    central_cubed = -DP_ORBIT_EARTH_MODEL.gravitational_parameter / r_cubed;

    for (index = 0u; index < 3u; ++index) {
        printf("ecef[%u] actual=%016llx expected=%016llx%s\n", index,
               (unsigned long long)dp_bits(ecef_data[index]),
               (unsigned long long)dp_bits(expected_ecef[index]),
               dp_bits(ecef_data[index]) == dp_bits(expected_ecef[index]) ? "" : " MISMATCH");
    }
    printf("sum=%016llx radius=%016llx r_times_sum=%016llx r_cubed=%016llx\n",
           (unsigned long long)dp_bits(sum), (unsigned long long)dp_bits(radius),
           (unsigned long long)dp_bits(r_times_sum), (unsigned long long)dp_bits(r_cubed));
    printf("central_sum=%016llx central_cubed=%016llx\n",
           (unsigned long long)dp_bits(central_sum), (unsigned long long)dp_bits(central_cubed));
    return 0;
}

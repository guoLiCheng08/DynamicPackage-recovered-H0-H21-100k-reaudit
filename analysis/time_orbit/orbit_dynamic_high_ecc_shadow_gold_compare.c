#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"
#include "dynamic_time.h"

static uint64_t dp_bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    static const uint64_t expected[3] = {
        UINT64_C(0x3f8a7e04672bb1bd),
        UINT64_C(0xbfb1f019465fb335),
        UINT64_C(0xbf9f02303c5d0ffb)
    };
    double position_data[3] = {-12000000.0, 65000000.0, 28000000.0};
    double actual_data[3] = {0.0, 0.0, 0.0};
    DpVector position = {3, 0, position_data};
    DpVector actual = {3, 0, actual_data};
    unsigned index;

    TimeInit(2031.0, 12.0, 31.0, 23.0, 59.0, 50.0);
    SpacecraftMass = 1000.0;
    F_I_external.data[0] = 0.0;
    F_I_external.data[1] = 0.0;
    F_I_external.data[2] = 0.0;
    orbit_dynamic(&actual, &position);
    for (index = 0u; index < 3u; ++index) {
        if (dp_bits(actual_data[index]) != expected[index]) {
            fprintf(stderr,
                    "orbit_dynamic high-ecc shadow field=%u actual=%a expected_bits=%016llx actual_bits=%016llx\n",
                    index, actual_data[index], (unsigned long long)expected[index],
                    (unsigned long long)dp_bits(actual_data[index]));
            return 1;
        }
    }
    puts("orbit_dynamic high-ecc shadow original-ELF compare: PASS (bitwise)");
    return 0;
}

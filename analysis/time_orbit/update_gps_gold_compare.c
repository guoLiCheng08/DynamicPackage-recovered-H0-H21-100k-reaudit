#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

static uint64_t dp_u64(double value)
{
    uint64_t bits;
    memcpy(&bits, &value, sizeof(bits));
    return bits;
}

int main(void)
{
    DpGpsKalmanRecovered gps;
    const double time_values[6] = {2025.9, -1.9, 3.1, 4.9, 5.0, 6.99};
    const DpVec3 position = {7000000.25, -123.5, 42.125};
    const DpVec3 velocity = {1.5, -2.25, 3.75};
    const uint64_t expected_doubles[6] = {
        UINT64_C(0x415ab3f010000000), UINT64_C(0xc05ee00000000000),
        UINT64_C(0x4045100000000000), UINT64_C(0x3ff8000000000000),
        UINT64_C(0xc002000000000000), UINT64_C(0x400e000000000000)
    };
    const int32_t expected_time[6] = {2025, -1, 3, 4, 5, 6};
    int init_flag = 1;
    unsigned index;

    memset(&gps, 0xa5, sizeof(gps));
    dp_update_gps(&gps, time_values, &position, &velocity, &init_flag);
    for (index = 0u; index < 3u; ++index) {
        if (dp_u64(gps.position_gci[index]) != expected_doubles[index]) {
            fprintf(stderr, "position[%u]: got %016" PRIx64 ", expected %016" PRIx64 "\n",
                    index, dp_u64(gps.position_gci[index]), expected_doubles[index]);
            return 1;
        }
        if (dp_u64(gps.velocity_gci[index]) != expected_doubles[index + 3u]) {
            fprintf(stderr, "velocity[%u]: got %016" PRIx64 ", expected %016" PRIx64 "\n",
                    index, dp_u64(gps.velocity_gci[index]), expected_doubles[index + 3u]);
            return 1;
        }
    }
    for (index = 0u; index < 6u; ++index) {
        if (gps.time_components[index] != expected_time[index]) {
            fprintf(stderr, "time[%u]: got %" PRId32 ", expected %" PRId32 "\n",
                    index, gps.time_components[index], expected_time[index]);
            return 1;
        }
    }
    if (init_flag != 0) {
        fprintf(stderr, "init_flag: got %d, expected 0\n", init_flag);
        return 1;
    }
    puts("UpdateGPS original-ELF gold compare: PASS (bitwise)");
    return 0;
}

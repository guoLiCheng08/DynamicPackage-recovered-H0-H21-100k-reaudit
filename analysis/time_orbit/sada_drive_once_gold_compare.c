#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static int compare_pair(const char *label, const double actual[2], const uint64_t expected[2])
{
    unsigned index;
    for (index = 0u; index < 2u; ++index) {
        if (bits(actual[index]) != expected[index]) {
            printf("FAIL %s[%u] actual=%a expected_bits=%016" PRIx64
                   " actual_bits=%016" PRIx64 "\n", label, index,
                   actual[index], expected[index], bits(actual[index]));
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    const uint64_t expected_angle[2] = {
        UINT64_C(0x3f60624dd2f1a9fd), UINT64_C(0xbf60624dd2f1a9fd)
    };
    const uint64_t expected_velocity[2] = {
        UINT64_C(0x3f947ae147ae147c), UINT64_C(0xbf947ae147ae147c)
    };
    const uint64_t expected_acceleration[2] = {
        UINT64_C(0x3fc999999999999b), UINT64_C(0xbfc999999999999b)
    };
    DpSadaRecovered sada;

    memset(&sada, 0, sizeof(sada));
    sada.command_flag = 1u;
    sada.command_angle[0] = 0.3;
    sada.command_angle[1] = -0.4;
    sada.command_limit[0] = 1.0;
    sada.command_limit[1] = 1.0;
    sada.acceleration_limit[0] = 0.2;
    sada.acceleration_limit[1] = 0.2;
    dp_drive_sada_once(&sada, 0.1);
    if (compare_pair("angle", sada.current_angle, expected_angle) != 0 ||
        compare_pair("velocity", sada.angular_velocity, expected_velocity) != 0 ||
        compare_pair("acceleration", sada.angular_acceleration, expected_acceleration) != 0) return 1;

    puts("SADA drive_SADA_once original-ELF gold compare: PASS (bitwise)");
    return 0;
}

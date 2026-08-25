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

int main(void)
{
    const uint64_t expected[6] = {
        UINT64_C(0x3fd0000000000000), UINT64_C(0xbff0000000000000),
        UINT64_C(0x4000000000000000), UINT64_C(0xc008000000000000),
        UINT64_C(0x0000000000000000), UINT64_C(0x4014000000000000)
    };
    DpMtqRecovered mtq[6];
    const double limits[6] = {0.5, 1.0, 2.0, 3.0, 4.0, 5.0};
    const double command[6] = {0.25, -2.0, 3.0, -4.0, 0.0, 6.0};
    unsigned index;

    memset(mtq, 0, sizeof(mtq));
    for (index = 0u; index < 6u; ++index) mtq[index].moment_limit = limits[index];
    dp_set_mtq_moment(mtq, command);
    for (index = 0u; index < 6u; ++index) {
        if (bits(mtq[index].actual_moment) != expected[index]) {
            printf("FAIL MTQ[%u] actual=%a expected_bits=%016" PRIx64
                   " actual_bits=%016" PRIx64 "\n", index, mtq[index].actual_moment,
                   expected[index], bits(mtq[index].actual_moment));
            return 1;
        }
    }
    puts("MTQ actuator original-ELF gold compare: PASS (bitwise)");
    return 0;
}

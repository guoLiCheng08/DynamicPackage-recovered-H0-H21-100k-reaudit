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
    const uint64_t expected_channel[6] = {
        UINT64_C(0x3fb999999999999a), UINT64_C(0xbfc999999999999a),
        UINT64_C(0x3fd3333333333333), UINT64_C(0xbfd999999999999a),
        UINT64_C(0x3fe0000000000000), UINT64_C(0xbfe3333333333333)
    };
    const uint64_t expected_group[3] = {
        UINT64_C(0x3fb999999999999a), UINT64_C(0xbfc999999999999a),
        UINT64_C(0x3fd3333333333333)
    };
    DpMtqRecovered mtq[6];
    const double moment[6] = {0.1, -0.2, 0.3, -0.4, 0.5, -0.6};
    double channel_data[6] = {0.0};
    double group_data[3] = {0.0};
    double mapping_data[18] = {
        1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0, 0.0
    };
    DpVector channel = {6, 0, channel_data};
    DpVector group = {3, 0, group_data};
    DpMatrix mapping = {3, 6, 6, 0, mapping_data};
    unsigned index;

    memset(mtq, 0, sizeof(mtq));
    for (index = 0u; index < 6u; ++index) mtq[index].actual_moment = moment[index];
    if (dp_update_mag_moment(&group, &channel, &mapping, mtq) != 0) {
        puts("FAIL UpdateMagMoment recovered function returned error");
        return 1;
    }
    for (index = 0u; index < 6u; ++index) {
        if (bits(channel_data[index]) != expected_channel[index]) {
            printf("FAIL channel[%u] expected_bits=%016" PRIx64 " actual_bits=%016" PRIx64 "\n",
                   index, expected_channel[index], bits(channel_data[index]));
            return 1;
        }
    }
    for (index = 0u; index < 3u; ++index) {
        if (bits(group_data[index]) != expected_group[index]) {
            printf("FAIL group[%u] expected_bits=%016" PRIx64 " actual_bits=%016" PRIx64 "\n",
                   index, expected_group[index], bits(group_data[index]));
            return 1;
        }
    }
    puts("UpdateMagMoment original-ELF gold compare: PASS (bitwise)");
    return 0;
}

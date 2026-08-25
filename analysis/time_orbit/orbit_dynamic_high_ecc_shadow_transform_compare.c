#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_time.h"

static uint64_t dp_bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    static const uint64_t expected_ecef[3] = {
        UINT64_C(0x418f85e4e3ed346a),
        UINT64_C(0x412b2b40c2174ba1),
        UINT64_C(0x417aaa9e82b94dfc)
    };
    double gci_data[3] = {-12000000.0, 65000000.0, 28000000.0};
    double ecef_data[3] = {0.0, 0.0, 0.0};
    DpVector gci = {3, 0, gci_data};
    DpVector ecef = {3, 0, ecef_data};
    DpCalendarTime calendar = {2031.0, 12.0, 31.0, 23.0, 59.0, 50.0};
    unsigned index;

    GCI2ECEF(&ecef, &gci, &calendar);
    for (index = 0u; index < 3u; ++index) {
        printf("ECEF[%u] bits=%016llx expected=%016llx\n", index,
               (unsigned long long)dp_bits(ecef_data[index]),
               (unsigned long long)expected_ecef[index]);
        if (dp_bits(ecef_data[index]) != expected_ecef[index]) return 1;
    }
    puts("GCI2ECEF high-ecc shadow original-ELF compare: PASS (bitwise)");
    return 0;
}

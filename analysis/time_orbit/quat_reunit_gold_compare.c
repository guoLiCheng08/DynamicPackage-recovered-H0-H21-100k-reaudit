#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static double from_bits(uint64_t value)
{
    double result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    const uint64_t input_bits[4] = {
        UINT64_C(0x3ff0000000000000), UINT64_C(0x3f30624dd2f1a9fc),
        UINT64_C(0xbf40624dd2f1a9fc), UINT64_C(0x3f489374bc6a7efa)
    };
    const uint64_t expected_bits[4] = {
        UINT64_C(0x3fefffff151e7f50), UINT64_C(0x3f30624d5aaf5bcf),
        UINT64_C(0xbf40624d5aaf5bcf), UINT64_C(0x3f489374080709b6)
    };
    double xyz[3];
    DpQuatAbi value;
    unsigned index;

    value.w = from_bits(input_bits[0]);
    value.xyz.count = 3;
    value.xyz.reserved_04 = 0;
    value.xyz.data = xyz;
    for (index = 0u; index < 3u; ++index) value.xyz.data[index] = from_bits(input_bits[index + 1u]);
    quat_reunit(&value);
    if (bits(value.w) != expected_bits[0]) {
        printf("FAIL q[0] expected=%016" PRIx64 " actual=%016" PRIx64 "\n", expected_bits[0], bits(value.w));
        return 1;
    }
    for (index = 0u; index < 3u; ++index) {
        if (bits(value.xyz.data[index]) != expected_bits[index + 1u]) {
            printf("FAIL q[%u] expected=%016" PRIx64 " actual=%016" PRIx64 "\n",
                   index + 1u, expected_bits[index + 1u], bits(value.xyz.data[index]));
            return 1;
        }
    }
    puts("quat_reunit non-unit original-ELF gold compare: PASS (bitwise)");
    return 0;
}

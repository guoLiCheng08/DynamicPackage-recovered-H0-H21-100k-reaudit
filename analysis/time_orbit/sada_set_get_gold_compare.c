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
    const uint64_t expected_command[2] = {
        UINT64_C(0x3fd3333333333333), UINT64_C(0xbfd999999999999a)
    };
    const uint64_t expected_current[2] = {
        UINT64_C(0x3ff4000000000000), UINT64_C(0xc004000000000000)
    };
    const double command[2] = {0.3, -0.4};
    DpSadaRecovered sada;
    double output[2] = {0.0, 0.0};
    unsigned index;

    memset(&sada, 0, sizeof(sada));
    dp_set_sada(&sada, 42u, command);
    if (sada.command_flag != 42u) {
        printf("FAIL SADA flag expected=42 actual=%u\n", sada.command_flag);
        return 1;
    }
    for (index = 0u; index < 2u; ++index) {
        if (bits(sada.command_angle[index]) != expected_command[index]) {
            printf("FAIL SADA command[%u] expected_bits=%016" PRIx64
                   " actual_bits=%016" PRIx64 "\n", index,
                   expected_command[index], bits(sada.command_angle[index]));
            return 1;
        }
    }
    sada.current_angle[0] = 1.25;
    sada.current_angle[1] = -2.5;
    dp_get_sada_angle(&sada, output);
    for (index = 0u; index < 2u; ++index) {
        if (bits(output[index]) != expected_current[index]) {
            printf("FAIL SADA current[%u] expected_bits=%016" PRIx64
                   " actual_bits=%016" PRIx64 "\n", index,
                   expected_current[index], bits(output[index]));
            return 1;
        }
    }
    puts("SADA SetSADA/getSADAangle original-ELF gold compare: PASS (bitwise)");
    return 0;
}

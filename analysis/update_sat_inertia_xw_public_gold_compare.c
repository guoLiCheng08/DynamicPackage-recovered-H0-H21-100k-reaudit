#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    static const uint64_t expected[18] = {
        UINT64_C(0x405e000000000000), UINT64_C(0xbfe428f5c28f5c29), UINT64_C(0x3ff4cccccccccccd),
        UINT64_C(0xbfe428f5c28f5c29), UINT64_C(0x4076900000000000), UINT64_C(0xbfe0000000000000),
        UINT64_C(0x3ff4cccccccccccd), UINT64_C(0xbfe0000000000000), UINT64_C(0x407d500000000000),
        UINT64_C(0x3f81113cdb6f56f3), UINT64_C(0x3eee6ed1eab39b43), UINT64_C(0xbef8349fe3dcda19),
        UINT64_C(0x3eee6ed1eab39b43), UINT64_C(0x3f66b158cdc4be55), UINT64_C(0x3ec86f97ee84395f),
        UINT64_C(0xbef8349fe3dcda19), UINT64_C(0x3ec86f97ee84395f), UINT64_C(0x3f6177ae1e1da471)
    };
    unsigned index;

    memset(J_c_B_mem, 0, sizeof(J_c_B_mem));
    memset(J_c_B_inv_mem, 0, sizeof(J_c_B_inv_mem));
    Update_sat_inertia_xw(1);
    for (index = 0u; index < 9u; ++index) {
        if (bits(J_c_B_mem[index]) != expected[index] || bits(J_c_B_inv_mem[index]) != expected[index + 9u]) {
            printf("FAIL index=%u J=%016llx expectedJ=%016llx Jinv=%016llx expectedJinv=%016llx\n",
                   index, (unsigned long long)bits(J_c_B_mem[index]),
                   (unsigned long long)expected[index],
                   (unsigned long long)bits(J_c_B_inv_mem[index]),
                   (unsigned long long)expected[index + 9u]);
            return 1;
        }
    }
    puts("Update_sat_inertia_xw public ABI flag1 original-ELF compare: PASS (bitwise J/Jinv)");
    return 0;
}

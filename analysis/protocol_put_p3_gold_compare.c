#include <stdio.h>
#include <string.h>

#include "dynamic_ipc_telemetry.h"

#define GOLD_PATH "analysis/time_orbit/gold_protocol_put_p3_output.bin"

int main(void)
{
    unsigned char actual[64];
    unsigned char expected[sizeof(actual)];
    FILE *file;
    size_t count;
    size_t index;

    memset(actual, 0xa5, sizeof(actual));
    file = fopen(GOLD_PATH, "rb");
    if (file == NULL) { perror(GOLD_PATH); return 1; }
    count = fread(expected, 1u, sizeof(expected), file);
    if (fclose(file) != 0 || count != sizeof(expected)) return 1;
    Put_UINT16_In_Buff(actual + 0u, 0x1234u, 0u);
    Put_UINT16_In_Buff(actual + 8u, 0x1234u, 1u);
    Put_UINT32_In_Buff(actual + 16u, 0x12345678u, 0u);
    Put_UINT32_In_Buff(actual + 24u, 0x12345678u, 1u);
    Put_FLOAT32_In_Buff(actual + 32u, 0u, 1.25f);
    Put_FLOAT32_In_Buff(actual + 40u, 1u, 1.25f);
    Put_FLOAT64_In_Buff(actual + 48u, 0u, -2.5);
    Put_FLOAT64_In_Buff(actual + 56u, 1u, -2.5);
    for (index = 0u; index < sizeof(actual); ++index) {
        if (actual[index] != expected[index]) {
            fprintf(stderr, "protocol put P3 mismatch +0x%zx: actual=%02x expected=%02x\n",
                    index, (unsigned)actual[index], (unsigned)expected[index]);
            return 1;
        }
    }
    puts("protocol fixed-width put P3 original-ELF compare: PASS (bitwise)");
    return 0;
}

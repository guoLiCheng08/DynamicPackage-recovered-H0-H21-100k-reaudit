#include <stdio.h>
#include <string.h>

#include "dynamic_ipc_telemetry.h"

#define GOLD_PATH "analysis/time_orbit/gold_change_endian_p3_output.bin"

int main(void)
{
    uint8_t source[8];
    uint8_t actual[16];
    uint8_t expected[16];
    FILE *file;
    size_t count;
    size_t index;

    for (index = 0u; index < 8u; ++index) {
        source[index] = (uint8_t)(0x10u + index);
        actual[index] = 0xa5u;
        actual[index + 8u] = 0x5au;
    }
    file = fopen(GOLD_PATH, "rb");
    if (file == NULL) { perror(GOLD_PATH); return 1; }
    count = fread(expected, 1u, sizeof(expected), file);
    if (fclose(file) != 0 || count != sizeof(expected)) return 1;
    Change_Endian(actual, source, 8);
    Change_Endian(actual + 8, source, 0);
    if (memcmp(actual, expected, sizeof(actual)) != 0) {
        for (index = 0u; index < sizeof(actual); ++index) {
            if (actual[index] != expected[index]) {
                fprintf(stderr, "Change_Endian P3 mismatch +0x%zx: actual=%02x expected=%02x\n",
                        index, (unsigned)actual[index], (unsigned)expected[index]);
                break;
            }
        }
        return 1;
    }
    puts("Change_Endian P3 original-ELF compare: PASS (bitwise)");
    return 0;
}

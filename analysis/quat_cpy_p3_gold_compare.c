#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

#define GOLD_PATH "analysis/time_orbit/gold_quat_cpy_p3_output.bin"

int main(void)
{
    double source[4] = {1.25, -0.0, 3.0, -4.5};
    double destination[4] = {99.0, 98.0, 97.0, 96.0};
    unsigned char expected[sizeof(destination)];
    FILE *file;
    size_t count;

    file = fopen(GOLD_PATH, "rb");
    if (file == NULL) { perror(GOLD_PATH); return 1; }
    count = fread(expected, 1u, sizeof(expected), file);
    if (fclose(file) != 0 || count != sizeof(expected)) return 1;
    quat_cpy(destination, source);
    if (memcmp(destination, expected, sizeof(destination)) != 0) {
        const unsigned char *actual = (const unsigned char *)destination;
        size_t index;
        for (index = 0u; index < sizeof(destination); ++index) {
            if (actual[index] != expected[index]) {
                fprintf(stderr, "quat_cpy P3 mismatch at +0x%zx: actual=%02x expected=%02x\n",
                        index, (unsigned)actual[index], (unsigned)expected[index]);
                break;
            }
        }
        return 1;
    }
    puts("quat_cpy P3 original-ELF compare: PASS (bitwise)");
    return 0;
}

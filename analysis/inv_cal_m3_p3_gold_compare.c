#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

int main(void)
{
    const double input[9] = {4.0, 1.0, 2.0, 0.0, 3.0, -1.0, 0.0, 0.0, 2.0};
    double actual[9] = {0};
    unsigned char expected[sizeof(actual)];
    FILE *file = fopen("analysis/time_orbit/inv_cal_m3_gold.bin", "rb");

    if (file == NULL || fread(expected, 1u, sizeof(expected), file) != sizeof(expected) ||
        fgetc(file) != EOF || fclose(file) != 0) {
        return 2;
    }
    inv_CAL_M3(input, actual);
    if (memcmp(actual, expected, sizeof(actual)) != 0) {
        fputs("inv_CAL_M3 P3 original-ELF mismatch\n", stderr);
        return 1;
    }
    puts("inv_CAL_M3 P3 original-ELF compare: PASS (bitwise)");
    return 0;
}

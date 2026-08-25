#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"

int main(void)
{
    static const double commands[6] = {0.5, 3.0, -4.0, 0.0, -1.5, 2.0};
    static const double limits[6] = {1.0, 2.0, 3.0, 4.0, 1.0, 1.5};
    double expected[6];
    double actual[6];
    FILE *file;
    unsigned index;

    file = fopen("analysis/coverage_inventory/gold_set_moment_mtq_p1_actual.bin", "rb");
    if (file == NULL || fread(expected, sizeof(expected), 1u, file) != 1u || fclose(file) != 0) return 2;
    dp_device_globals_reset();
    for (index = 0; index < 6u; ++index) MTQ[index].moment_limit = limits[index];
    SetMomentMTQ(commands);
    for (index = 0; index < 6u; ++index) actual[index] = MTQ[index].actual_moment;
    if (memcmp(actual, expected, sizeof(expected)) != 0) return 1;
    puts("SetMomentMTQ public ABI original-ELF compare: PASS (bitwise, 6-channel limits + zero)");
    return 0;
}

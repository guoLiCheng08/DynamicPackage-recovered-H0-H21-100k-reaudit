#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"
#include "dynamic_satellite_globals.h"

int main(void)
{
    double expected[2];
    double actual[2] = {123.0, -456.0};
    FILE *file = fopen("analysis/coverage_inventory/gold_get_sada_angle_p1_out.bin", "rb");

    if (file == NULL || fread(expected, sizeof(expected), 1u, file) != 1u || fclose(file) != 0) return 2;
    dp_device_globals_reset();
    SADA.current_angle[0] = -1.23456789012345;
    SADA.current_angle[1] = 9.87654321098765;
    getSADAangle(actual);
    if (memcmp(actual, expected, sizeof(expected)) != 0) return 1;
    puts("getSADAangle public ABI original-ELF compare: PASS (bitwise, 2-double global copy)");
    return 0;
}

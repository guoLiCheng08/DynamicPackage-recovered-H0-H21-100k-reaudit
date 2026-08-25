#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

int main(void)
{
    double sun_data[3] = {1.0, 0.0, 0.0};
    double position_data[3] = {7000000.0, 0.0, 0.0};
    DpVector sun = {3, 0, sun_data};
    DpVector position = {3, 0, position_data};
    int expected[2];
    int actual[2];
    FILE *file = fopen("analysis/coverage_inventory/gold_is_earth_shadow_p1_returns.bin", "rb");

    if (file == NULL || fread(expected, sizeof(expected), 1u, file) != 1u || fclose(file) != 0) return 2;
    actual[0] = isEarthShadow(&sun, &position);
    sun_data[0] = -1.0;
    actual[1] = isEarthShadow(&sun, &position);
    if (memcmp(actual, expected, sizeof(expected)) != 0) return 1;
    puts("isEarthShadow public ABI original-ELF compare: PASS (bitwise, sun-aligned + anti-aligned)");
    return 0;
}

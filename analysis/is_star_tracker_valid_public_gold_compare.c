#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

int main(void)
{
    double qxyz[3] = {0.0, 0.0, 0.0};
    double position_data[3] = {7000000.0, 0.0, 0.0};
    double sun_data[3] = {0.0, 0.0, 1.0};
    double installation_data[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    DpQuatAbi attitude = {1.0, {3, 0, qxyz}};
    DpVector position = {3, 0, position_data};
    DpVector sun = {3, 0, sun_data};
    DpMatrix installation = {3, 3, 3, 0, installation_data};
    int expected[2];
    int actual[2];
    FILE *file = fopen("analysis/coverage_inventory/gold_is_star_tracker_valid_p1_returns.bin", "rb");

    if (file == NULL || fread(expected, sizeof(expected), 1u, file) != 1u || fclose(file) != 0) return 2;
    actual[0] = isStarTrackerValid(&attitude, &position, &sun, &installation, 0.2, 0.1);
    sun_data[2] = -1.0;
    actual[1] = isStarTrackerValid(&attitude, &position, &sun, &installation, 0.2, 0.1);
    if (memcmp(actual, expected, sizeof(expected)) != 0) return 1;
    puts("isStarTrackerValid public ABI original-ELF compare: PASS (bitwise, sun-visible + earth-blocked)");
    return 0;
}

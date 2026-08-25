#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynamic_devices.h"

static int read_double(const char *path, double *value)
{
    FILE *file = fopen(path, "rb");
    return file != NULL && fread(value, sizeof(*value), 1u, file) == 1u && fclose(file) == 0;
}

int main(void)
{
    double expected_no_noise;
    double expected_seeded_noise;
    double actual;

    if (!read_double("analysis/coverage_inventory/gold_get_wheel_speed_p1_no_noise.bin", &expected_no_noise) ||
        !read_double("analysis/coverage_inventory/gold_get_wheel_speed_p1_seeded_noise.bin", &expected_seeded_noise)) return 2;
    memset(RWheel, 0, sizeof(RWheel));
    RWheel[2].add_gaussian_noise_flag = 0u;
    RWheel[2].omega = -12.75;
    actual = GetWheelSpeed(2);
    if (memcmp(&actual, &expected_no_noise, sizeof(actual)) != 0) return 1;
    RWheel[2].add_gaussian_noise_flag = 1u;
    RWheel[2].telemetry_noise_sigma = 0.25;
    srand(1);
    actual = GetWheelSpeed(2);
    if (memcmp(&actual, &expected_seeded_noise, sizeof(actual)) != 0) return 1;
    puts("GetWheelSpeed public ABI original-ELF compare: PASS (bitwise, no-noise + seeded-noise)");
    return 0;
}

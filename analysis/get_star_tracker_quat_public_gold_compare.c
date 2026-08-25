#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_sensors.h"

static int read_quat(const char *path, double out[4])
{
    FILE *file = fopen(path, "rb");
    return file != NULL && fread(out, sizeof(*out), 4u, file) == 4u && fclose(file) == 0;
}

static void seed_sts0(double xyz[3])
{
    memset(STS, 0, sizeof(STS));
    xyz[0] = -0.25;
    xyz[1] = 0.375;
    xyz[2] = -0.5;
    STS[0].measure_quat.w = 0.75;
    STS[0].measure_quat.xyz.count = 3;
    STS[0].measure_quat.xyz.data = xyz;
}

int main(void)
{
    double xyz[3];
    double expected[4];
    double actual[4] = {0.0, 0.0, 0.0, 0.0};

    if (!read_quat("analysis/coverage_inventory/gold_get_star_tracker_quat_p1_no_noise.bin", expected)) return 2;
    seed_sts0(xyz);
    STS[0].gaussian_noise_flag = 0u;
    GetStarTrackerQuat(actual, 0);
    if (memcmp(actual, expected, sizeof(expected)) != 0) return 1;

    if (!read_quat("analysis/coverage_inventory/gold_get_star_tracker_quat_p1_seed1_noise.bin", expected)) return 2;
    seed_sts0(xyz);
    STS[0].gaussian_noise_flag = 1u;
    STS[0].euler_mean_deg[0] = 0.1;
    STS[0].euler_mean_deg[1] = 0.2;
    STS[0].euler_mean_deg[2] = -0.3;
    STS[0].euler_sigma_deg[0] = 0.04;
    STS[0].euler_sigma_deg[1] = 0.05;
    STS[0].euler_sigma_deg[2] = 0.06;
    srand(1u);
    GetStarTrackerQuat(actual, 0);
    if (memcmp(actual, expected, sizeof(expected)) != 0) return 1;
    puts("GetStarTrackerQuat public ABI original-ELF compare: PASS (bitwise, no-noise + srand(1) noise)");
    return 0;
}

#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

int main(void)
{
    double error_xyz[3][3] = {{0.01, -0.02, 0.03}, {0.02, -0.04, 0.06}, {0.03, -0.06, 0.09}};
    double measure_xyz[3][3] = {{0.0}};
    double truth_xyz[3] = {0.1, -0.2, 0.3};
    DpQuatAbi truth = {1.0, {3, 0, truth_xyz}};
    double expected[15];
    double actual[15];
    FILE *file;
    unsigned index;

    file = fopen("analysis/coverage_inventory/gold_update_sts_quat_p1_state.bin", "rb");
    if (file == NULL || fread(expected, sizeof(expected), 1u, file) != 1u || fclose(file) != 0) return 2;
    memset(STS, 0, sizeof(STS));
    for (index = 0u; index < 3u; ++index) {
        STS[index].update_period = 1.0;
        STS[index].error_quat.w = 1.0;
        STS[index].error_quat.xyz.count = 3;
        STS[index].error_quat.xyz.data = error_xyz[index];
        STS[index].measure_quat.w = 0.0;
        STS[index].measure_quat.xyz.count = 3;
        STS[index].measure_quat.xyz.data = measure_xyz[index];
    }
    Update_STS_Quat(&truth, 0.25);
    for (index = 0u; index < 3u; ++index) {
        actual[index * 5u] = STS[index].measure_quat.w;
        actual[index * 5u + 1u] = measure_xyz[index][0];
        actual[index * 5u + 2u] = measure_xyz[index][1];
        actual[index * 5u + 3u] = measure_xyz[index][2];
        actual[index * 5u + 4u] = STS[index].accumulated_time;
    }
    if (memcmp(actual, expected, sizeof(expected)) != 0) return 1;
    puts("Update_STS_Quat public ABI original-ELF compare: PASS (bitwise, 3-channel trigger + time)");
    return 0;
}

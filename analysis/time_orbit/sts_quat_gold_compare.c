#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static int compare_quat(const char *label, const DpQuatAbi *actual)
{
    const uint64_t expected[4] = {
        UINT64_C(0x3ff0000000000000), UINT64_C(0x3fb999999999999a),
        UINT64_C(0xbfc999999999999a), UINT64_C(0x3fd3333333333333)
    };
    const double values[4] = {actual->w, actual->xyz.data[0], actual->xyz.data[1], actual->xyz.data[2]};
    unsigned index;
    for (index = 0u; index < 4u; ++index) {
        if (bits(values[index]) != expected[index]) {
            printf("FAIL %s[%u] actual=%a expected_bits=%016" PRIx64
                   " actual_bits=%016" PRIx64 "\n", label, index, values[index],
                   expected[index], bits(values[index]));
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    DpStsRecovered sensors[3];
    double error_xyz[3][3] = {{0.0}};
    double measure_xyz[3][3] = {{0.0}};
    double truth_xyz[3] = {0.1, -0.2, 0.3};
    DpQuatAbi truth = {1.0, {3, 0, truth_xyz}};
    unsigned index;

    memset(sensors, 0, sizeof(sensors));
    for (index = 0u; index < 3u; ++index) {
        sensors[index].update_period = 1.0;
        sensors[index].accumulated_time = 0.0;
        sensors[index].error_quat = (DpQuatAbi){1.0, {3, 0, error_xyz[index]}};
        sensors[index].measure_quat = (DpQuatAbi){0.0, {3, 0, measure_xyz[index]}};
    }
    dp_update_sts_quat(sensors, &truth, 0.1);
    for (index = 0u; index < 3u; ++index) {
        if (compare_quat("STS measure", &sensors[index].measure_quat) != 0 ||
            bits(sensors[index].accumulated_time) != UINT64_C(0x3fb999999999999a)) {
            printf("FAIL STS time[%u] bits=%016" PRIx64 "\n", index,
                   bits(sensors[index].accumulated_time));
            return 1;
        }
    }
    puts("STS Update_STS_Quat original-ELF gold compare: PASS (bitwise)");
    return 0;
}

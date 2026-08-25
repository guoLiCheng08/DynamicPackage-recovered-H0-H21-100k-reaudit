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

static int compare_block(const char *label, const double *actual,
                         const uint64_t expected[3])
{
    unsigned index;
    for (index = 0u; index < 3u; ++index) {
        if (bits(actual[index]) != expected[index]) {
            printf("FAIL %s[%u] actual=%a expected_bits=%016" PRIx64
                   " actual_bits=%016" PRIx64 "\n", label, index,
                   actual[index], expected[index], bits(actual[index]));
            return 1;
        }
    }
    return 0;
}

static int compare_scalar(const char *label, double actual, uint64_t expected)
{
    if (bits(actual) != expected) {
        printf("FAIL %s actual=%a expected_bits=%016" PRIx64 " actual_bits=%016" PRIx64 "\n",
               label, actual, expected, bits(actual));
        return 1;
    }
    return 0;
}

int main(void)
{
    const uint64_t expected_measure_0[3] = {
        UINT64_C(0x3fe3333333333334), UINT64_C(0x3fe999999999999a), UINT64_C(0x3ff0000000000000)
    };
    const uint64_t expected_measure_1[3] = {
        UINT64_C(0x3ff5555555555555), UINT64_C(0x3ffaaaaaaaaaaaaa), UINT64_C(0x3ff0000000000000)
    };
    DpDssRecovered sensors[2];
    double matrix_0[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    double matrix_1[9] = {0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0};
    double measure_0[3] = {0.0};
    double measure_1[3] = {0.0};
    DpVec3 sun_body = {3.0, 4.0, 5.0};

    memset(sensors, 0, sizeof(sensors));
    sensors[0].projection_matrix = (DpMatrix){3, 3, 3, 0, matrix_0};
    sensors[1].projection_matrix = (DpMatrix){3, 3, 3, 0, matrix_1};
    sensors[0].measure = (DpVector){3, 0, measure_0};
    sensors[1].measure = (DpVector){3, 0, measure_1};
    dp_update_dss(sensors, &sun_body, NULL, NULL);

    if (compare_block("dss[0].measure", measure_0, expected_measure_0) != 0 ||
        compare_block("dss[1].measure", measure_1, expected_measure_1) != 0 ||
        compare_scalar("dss[0].angle_x", sensors[0].angle_x, UINT64_C(0x3fe14b1dd5f90ce2)) != 0 ||
        compare_scalar("dss[0].ratio_x", sensors[0].ratio_x, UINT64_C(0x3fe3333333333334)) != 0 ||
        compare_scalar("dss[0].angle_y", sensors[0].angle_y, UINT64_C(0x3fe5977a5103ea93)) != 0 ||
        compare_scalar("dss[0].ratio_y", sensors[0].ratio_y, UINT64_C(0x3fe999999999999a)) != 0 ||
        compare_scalar("dss[1].angle_x", sensors[1].angle_x, UINT64_C(0x3fedac670561bb4f)) != 0 ||
        compare_scalar("dss[1].ratio_x", sensors[1].ratio_x, UINT64_C(0x3ff5555555555555)) != 0 ||
        compare_scalar("dss[1].angle_y", sensors[1].angle_y, UINT64_C(0x3ff07c6c6947a6a8)) != 0 ||
        compare_scalar("dss[1].ratio_y", sensors[1].ratio_y, UINT64_C(0x3ffaaaaaaaaaaaaa)) != 0) return 1;
    puts("DSS original-ELF gold compare: PASS (bitwise)");
    return 0;
}

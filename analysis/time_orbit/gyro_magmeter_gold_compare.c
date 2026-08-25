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

int main(void)
{
    const uint64_t expected_gyro_0[3] = {
        UINT64_C(0x3ff4000000000000), UINT64_C(0xc004000000000000), UINT64_C(0x400e000000000000)
    };
    const uint64_t expected_gyro_1[3] = {
        UINT64_C(0xc004000000000000), UINT64_C(0x400e000000000000), UINT64_C(0x3ff4000000000000)
    };
    const uint64_t expected_mag_0[3] = {
        UINT64_C(0x4010000000000000), UINT64_C(0xc014000000000000), UINT64_C(0x4018000000000000)
    };
    const uint64_t expected_mag_1[3] = {
        UINT64_C(0x4018000000000000), UINT64_C(0x4010000000000000), UINT64_C(0xc014000000000000)
    };
    DpGyroRecovered gyros[2];
    DpMagMeterRecovered meters[2];
    double gyro_matrix_0[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    double gyro_matrix_1[9] = {0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0};
    double mag_matrix_0[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    double mag_matrix_1[9] = {0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    double gyro_measure_0[3] = {0.0};
    double gyro_measure_1[3] = {0.0};
    double mag_measure_0[3] = {0.0};
    double mag_measure_1[3] = {0.0};
    DpVec3 body_rate = {1.25, -2.5, 3.75};
    DpVec3 magnetic_body = {4.0, -5.0, 6.0};

    memset(gyros, 0, sizeof(gyros));
    memset(meters, 0, sizeof(meters));
    gyros[0].projection_matrix = (DpMatrix){3, 3, 3, 0, gyro_matrix_0};
    gyros[1].projection_matrix = (DpMatrix){3, 3, 3, 0, gyro_matrix_1};
    gyros[0].measure = (DpVector){3, 0, gyro_measure_0};
    gyros[1].measure = (DpVector){3, 0, gyro_measure_1};
    meters[0].projection_matrix = (DpMatrix){3, 3, 3, 0, mag_matrix_0};
    meters[1].projection_matrix = (DpMatrix){3, 3, 3, 0, mag_matrix_1};
    meters[0].measure = (DpVector){3, 0, mag_measure_0};
    meters[1].measure = (DpVector){3, 0, mag_measure_1};

    dp_update_gyro(gyros, &body_rate, NULL, NULL);
    dp_update_magmeter(meters, &magnetic_body, NULL, NULL);
    if (compare_block("gyro[0]", gyro_measure_0, expected_gyro_0) != 0 ||
        compare_block("gyro[1]", gyro_measure_1, expected_gyro_1) != 0 ||
        compare_block("magmeter[0]", mag_measure_0, expected_mag_0) != 0 ||
        compare_block("magmeter[1]", mag_measure_1, expected_mag_1) != 0) return 1;
    puts("Gyro/MagMeter original-ELF gold compare: PASS (bitwise)");
    return 0;
}

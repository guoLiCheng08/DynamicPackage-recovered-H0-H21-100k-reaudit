#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

static double dp_f64(uint64_t bits)
{
    double value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

static void setup_sensor(DpStsRecovered *sensor, double matrix_data[9], const uint64_t matrix_bits[9])
{
    unsigned index;
    memset(sensor, 0, sizeof(*sensor));
    sensor->sun_exclusion_angle = dp_f64(UINT64_C(0x3fe38c35418a5bf6));
    sensor->earth_margin_angle = dp_f64(UINT64_C(0x3fdbecde5da115a9));
    for (index = 0u; index < 9u; ++index) matrix_data[index] = dp_f64(matrix_bits[index]);
    sensor->installation_matrix.rows = 3;
    sensor->installation_matrix.cols = 3;
    sensor->installation_matrix.row_stride = 3;
    sensor->installation_matrix.data = matrix_data;
}

int main(void)
{
    static const uint64_t matrix_bits[3][9] = {
        {UINT64_C(0x0000000000000000), UINT64_C(0xbff0000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xbfd6f0068db8bac7), UINT64_C(0x0000000000000000), UINT64_C(0xbfede00d1b71758e), UINT64_C(0x3fede00d1b71758e), UINT64_C(0x0000000000000000), UINT64_C(0xbfd6f0068db8bac7)},
        {UINT64_C(0xbfd43fe5c91d14e4), UINT64_C(0x3fde0c49ba5e353f), UINT64_C(0xbfea60aa64c2f838), UINT64_C(0x3fc58793dd97f62b), UINT64_C(0x3fec40b780346dc6), UINT64_C(0x3fdc0d1b71758e22), UINT64_C(0x3fede00d1b71758e), UINT64_C(0x0000000000000000), UINT64_C(0xbfd6f0068db8bac7)},
        {UINT64_C(0x3fd43fe5c91d14e4), UINT64_C(0x3fde0c49ba5e353f), UINT64_C(0x3fea60aa64c2f838), UINT64_C(0x3fc58793dd97f62b), UINT64_C(0xbfec40b780346dc6), UINT64_C(0x3fdc0d1b71758e22), UINT64_C(0x3fede00d1b71758e), UINT64_C(0x0000000000000000), UINT64_C(0xbfd6f0068db8bac7)}
    };
    double matrix_data[3][9];
    double q_xyz[3] = {0.0, 0.0, 0.0};
    DpQuatAbi identity = {1.0, {3, 0, q_xyz}};
    double position_data[3] = {7000000.0, 0.0, 0.0};
    DpVector position = {3, 0, position_data};
    double sun_data[3] = {0.0, 0.0, 0.0};
    DpVector sun = {3, 0, sun_data};
    static const double directions[4][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {-1.0, 0.0, 0.0}};
    int expected[12];
    int actual[12];
    FILE *file;
    unsigned test_index;
    unsigned sensor_index;

    file = fopen("analysis/coverage_inventory/gold_update_sts_valid_flag_p1_flags.bin", "rb");
    if (file == NULL || fread(expected, sizeof(expected), 1u, file) != 1u || fclose(file) != 0) return 2;
    for (sensor_index = 0u; sensor_index < 3u; ++sensor_index) setup_sensor(&STS[sensor_index], matrix_data[sensor_index], matrix_bits[sensor_index]);
    for (test_index = 0u; test_index < 4u; ++test_index) {
        sun_data[0] = directions[test_index][0];
        sun_data[1] = directions[test_index][1];
        sun_data[2] = directions[test_index][2];
        Update_STS_ValidFlag(&identity, &position, &sun);
        for (sensor_index = 0u; sensor_index < 3u; ++sensor_index) actual[test_index * 3u + sensor_index] = STS[sensor_index].valid_flag;
    }
    if (memcmp(actual, expected, sizeof(actual)) != 0) return 1;
    puts("Update_STS_ValidFlag public ABI original-ELF compare: PASS (bitwise, 4 directions x 3 STS)");
    return 0;
}

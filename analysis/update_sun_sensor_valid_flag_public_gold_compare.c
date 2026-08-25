#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

int main(void)
{
    double measure_data[2][3] = {{0.0}};
    double sun_data[3] = {1.0, 0.0, 0.0};
    double position_data[3] = {7000000.0, 0.0, 0.0};
    DpVector sun = {3, 0, sun_data};
    DpVector position = {3, 0, position_data};
    int expected[4];
    int actual[4];
    FILE *file;
    unsigned index;

    file = fopen("analysis/coverage_inventory/gold_update_sun_sensor_valid_flag_p1_flags.bin", "rb");
    if (file == NULL || fread(expected, sizeof(expected), 1u, file) != 1u || fclose(file) != 0) return 2;
    memset(DSS, 0, sizeof(DSS));
    for (index = 0u; index < 2u; ++index) {
        DSS[index].valid_angle_threshold = 100.0;
        DSS[index].measure.count = 3;
        DSS[index].measure.data = measure_data[index];
    }
    UpdateSunSensorValidFlag(&sun, &position);
    actual[0] = DSS[0].valid_flag;
    actual[1] = DSS[1].valid_flag;
    sun_data[0] = -1.0;
    UpdateSunSensorValidFlag(&sun, &position);
    actual[2] = DSS[0].valid_flag;
    actual[3] = DSS[1].valid_flag;
    if (memcmp(actual, expected, sizeof(expected)) != 0) return 1;
    puts("UpdateSunSensorValidFlag public ABI original-ELF compare: PASS (bitwise, two DSS flags + shadow branches)");
    return 0;
}

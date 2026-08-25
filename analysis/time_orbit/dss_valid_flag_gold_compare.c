#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

int main(void)
{
    DpDssRecovered sensors[2];
    double measurement_data[2][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    double position_data[3] = {7000000.0, 0.0, 0.0};
    double sun_data[3] = {1.0, 0.0, 0.0};
    DpVector position = {3, 0, position_data};
    DpVector sun = {3, 0, sun_data};
    const double directions[3][3] = {{1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}};
    const int32_t expected[3] = {0, 1, 0};
    unsigned direction_index;
    unsigned sensor_index;

    memset(sensors, 0, sizeof(sensors));
    for (sensor_index = 0u; sensor_index < 2u; ++sensor_index) {
        sensors[sensor_index].valid_angle_threshold = 0.1;
        sensors[sensor_index].measure.count = 3;
        sensors[sensor_index].measure.data = measurement_data[sensor_index];
    }
    for (direction_index = 0u; direction_index < 3u; ++direction_index) {
        sun_data[0] = directions[direction_index][0];
        sun_data[1] = directions[direction_index][1];
        sun_data[2] = directions[direction_index][2];
        if (dp_is_earth_shadow(&sun, &position) != expected[direction_index]) {
            fprintf(stderr, "isEarthShadow direction %u mismatch\n", direction_index);
            return 1;
        }
        dp_update_dss_valid_flag(sensors, &sun, &position);
        for (sensor_index = 0u; sensor_index < 2u; ++sensor_index) {
            if (sensors[sensor_index].valid_flag != expected[direction_index]) {
                fprintf(stderr, "DSS direction %u sensor %u: got %d expected %d\n",
                        direction_index, sensor_index, sensors[sensor_index].valid_flag,
                        expected[direction_index]);
                return 1;
            }
        }
    }
    puts("UpdateSunSensorValidFlag/isEarthShadow original-ELF gold compare: PASS");
    return 0;
}

#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

int main(void)
{
    static const double time_values[6] = {2025.9, -1.9, 3.1, 4.9, -5.1, 6.99};
    const DpVec3 position = {7000000.25, -123.5, 42.125};
    const DpVec3 velocity = {1.5, -2.25, 3.75};
    unsigned char expected_tail[0x48];
    int expected_flag;
    FILE *file;

    file = fopen("analysis/coverage_inventory/gold_update_gps_p1_tail.bin", "rb");
    if (file == NULL || fread(expected_tail, sizeof(expected_tail), 1u, file) != 1u || fclose(file) != 0) return 2;
    file = fopen("analysis/coverage_inventory/gold_update_gps_p1_init_flag.bin", "rb");
    if (file == NULL || fread(&expected_flag, sizeof(expected_flag), 1u, file) != 1u || fclose(file) != 0) return 2;
    memset(&GPS_Kalman, 0, sizeof(GPS_Kalman));
    init_flag = 1;
    UpdateGPS(time_values, &position, &velocity);
    if (memcmp((const unsigned char *)&GPS_Kalman + 0x28, expected_tail, sizeof(expected_tail)) != 0 ||
        memcmp(&init_flag, &expected_flag, sizeof(expected_flag)) != 0) return 1;
    puts("UpdateGPS public ABI original-ELF compare: PASS (bitwise, global tail + init_flag)");
    return 0;
}

#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

int main(void)
{
    double matrices[2][9] = {{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                             {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}};
    double measures[2][3] = {{0.0}};
    double field_data[3] = {-0.875, 4.5, 0.0625};
    DpVector field = {3, 0, field_data};
    double expected[6];
    double actual[6];
    FILE *file;
    unsigned index;

    file = fopen("analysis/coverage_inventory/gold_update_magmeter_p1_measure.bin", "rb");
    if (file == NULL || fread(expected, sizeof(expected), 1u, file) != 1u || fclose(file) != 0) return 2;
    memset(MagMeter, 0, sizeof(MagMeter));
    for (index = 0u; index < 2u; ++index) {
        MagMeter[index].projection_matrix.rows = 3;
        MagMeter[index].projection_matrix.cols = 3;
        MagMeter[index].projection_matrix.row_stride = 3;
        MagMeter[index].projection_matrix.data = matrices[index];
        MagMeter[index].measure.count = 3;
        MagMeter[index].measure.data = measures[index];
    }
    UpdateMagMeter(&field);
    for (index = 0u; index < 2u; ++index) {
        actual[index * 3u] = measures[index][0];
        actual[index * 3u + 1u] = measures[index][1];
        actual[index * 3u + 2u] = measures[index][2];
    }
    if (memcmp(actual, expected, sizeof(actual)) != 0) return 1;
    puts("UpdateMagMeter public ABI original-ELF compare: PASS (bitwise, 2-channel projection)");
    return 0;
}

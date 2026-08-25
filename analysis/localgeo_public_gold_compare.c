#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_environment.h"

static int compare_gold(const char *label, const void *actual, size_t byte_count,
                        const char *gold_path)
{
    unsigned char expected[9u * sizeof(double)];
    const unsigned char *actual_bytes = actual;
    FILE *gold_file;
    size_t index;

    if (byte_count > sizeof(expected)) return 1;
    gold_file = fopen(gold_path, "rb");
    if (gold_file == NULL) return 1;
    if (fread(expected, 1u, byte_count, gold_file) != byte_count ||
        fgetc(gold_file) != EOF) {
        (void)fclose(gold_file);
        return 1;
    }
    (void)fclose(gold_file);
    if (memcmp(actual, expected, byte_count) == 0) return 0;
    for (index = 0u; index < byte_count; ++index) {
        if (actual_bytes[index] != expected[index]) {
            (void)fprintf(stderr, "%s byte %zu mismatch: got=%#x expected=%#x\n",
                          label, index, actual_bytes[index], expected[index]);
            return 1;
        }
    }
    return 1;
}

int main(void)
{
    double rotation_data[9] = {0.0};
    double position_data[3] = {4.0, -3.0, 12.0};
    double input_data[3] = {7.0, -11.0, 13.0};
    double reference_data[3] = {1.5, -2.0, 0.5};
    double lvlh_data[3] = {0.0, 0.0, 0.0};
    DpMatrix rotation = {3, 3, 3, 0, rotation_data};
    DpVector position = {3, 0, position_data};
    DpVector input = {3, 0, input_data};
    DpVector reference = {3, 0, reference_data};
    DpVector lvlh = {3, 0, lvlh_data};

    R_ECEF2LocalGeo(&rotation, &position);
    if (compare_gold("R_ECEF2LocalGeo", rotation_data, sizeof(rotation_data),
                     "analysis/time_orbit/recef2localgeo_gold.bin") != 0) {
        return 1;
    }
    LocalGeo2LVLH(&lvlh, &input, &position, &reference);
    if (compare_gold("LocalGeo2LVLH", lvlh_data, sizeof(lvlh_data),
                     "analysis/time_orbit/localgeo2lvlh_gold.bin") != 0) return 1;
    (void)puts("PASS: local geographic transforms controlled original-ELF gold compare");
    return 0;
}

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

static uint64_t bits(double value)
{
    uint64_t result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    uint64_t neg_nan_bits = UINT64_C(0xfff8000000000000);
    double q_xyz[3];
    DpQuatAbi quat;
    double matrix_data[9] = {0.0};
    DpMatrix matrix = {3, 3, 3, 0, matrix_data};
    double expected[9];
    FILE *file;
    unsigned index;

    memcpy(&quat.w, &neg_nan_bits, sizeof(quat.w));
    for (index = 0u; index < 3u; ++index) {
        memcpy(&q_xyz[index], &neg_nan_bits, sizeof(q_xyz[index]));
    }
    quat.xyz.count = 3;
    quat.xyz.reserved_04 = 0;
    quat.xyz.data = q_xyz;

    file = fopen("analysis/time_orbit/gold_quat_att_mat_h0_step101_neg_nan_matrix.bin", "rb");
    if (file == NULL || fread(expected, 1u, sizeof(expected), file) != sizeof(expected) ||
        fclose(file) != 0) {
        return 2;
    }

    quat_att_mat(&matrix, &quat);
    file = fopen("/tmp/quat_att_mat_h0_step101_actual_matrix.bin", "wb");
    if (file == NULL || fwrite(matrix_data, 1u, sizeof(matrix_data), file) != sizeof(matrix_data) ||
        fclose(file) != 0) {
        return 2;
    }
    for (index = 0u; index < 9u; ++index) {
        if (bits(matrix_data[index]) != bits(expected[index])) {
            printf("quat_att_mat H0 step101 neg-NaN[%u] actual=%016" PRIx64
                   " expected=%016" PRIx64 "\n",
                   index, bits(matrix_data[index]), bits(expected[index]));
            return 1;
        }
    }
    puts("quat_att_mat H0 step101 neg-NaN original-ELF compare: PASS (bitwise)");
    return 0;
}

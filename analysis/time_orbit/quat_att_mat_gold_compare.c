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
    const uint64_t expected[9] = {
        UINT64_C(0x3fec28f5c28f5c29), UINT64_C(0x3fe1eb851eb851eb), UINT64_C(0x3fdd70a3d70a3d72),
        UINT64_C(0xbfe47ae147ae147b), UINT64_C(0x3fee147ae147ae15), UINT64_C(0x3fb47ae147ae147c),
        UINT64_C(0xbfd5c28f5c28f5c2), UINT64_C(0xbfd47ae147ae147b), UINT64_C(0x3ff0a3d70a3d70a4)
    };
    double q_xyz[3] = {0.1, -0.2, 0.3};
    DpQuatAbi quat = {1.0, {3, 0, q_xyz}};
    double matrix_data[9] = {0.0};
    DpMatrix matrix = {3, 3, 3, 0, matrix_data};
    unsigned index;

    quat_att_mat(&matrix, &quat);
    for (index = 0u; index < 9u; ++index) {
        if (bits(matrix_data[index]) != expected[index]) {
            printf("FAIL quat_att_mat[%u] actual=%a bits=%016" PRIx64
                   " expected=%016" PRIx64 "\n", index, matrix_data[index],
                   bits(matrix_data[index]), expected[index]);
            return 1;
        }
    }
    puts("quat_att_mat original-ELF gold compare: PASS (bitwise)");
    return 0;
}

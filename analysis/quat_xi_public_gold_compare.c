#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

int main(void)
{
    double xyz[3] = {-2.5, 3.75, -4.5};
    double preserved_xyz[3];
    double matrix_data[12];
    double expected[12];
    DpQuatAbi quat = {1.25, {3, 0, xyz}};
    DpMatrix matrix = {4, 3, 3, 0, matrix_data};
    FILE *file = fopen("analysis/time_orbit/quat_xi_gold.bin", "rb");

    if (file == NULL || fread(expected, 1u, sizeof(expected), file) != sizeof(expected) ||
        fclose(file) != 0) {
        (void)puts("cannot load quat_xi original-ELF gold");
        return 1;
    }
    memset(matrix_data, 0xa5, sizeof(matrix_data));
    memcpy(preserved_xyz, xyz, sizeof(xyz));
    quat_xi(&matrix, &quat);
    if (memcmp(matrix_data, expected, sizeof(matrix_data)) != 0 ||
        memcmp(xyz, preserved_xyz, sizeof(xyz)) != 0) {
        (void)puts("quat_xi original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: quat_xi controlled original-ELF gold compare");
    return 0;
}

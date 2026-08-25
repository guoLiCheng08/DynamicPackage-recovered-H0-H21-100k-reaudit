#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

int main(void)
{
    double xyz[3] = {3.0, 4.0, 12.0};
    double preserved_xyz[3];
    double expected;
    double actual;
    DpQuatAbi quat = {0.0, {3, 0, xyz}};
    FILE *file = fopen("analysis/time_orbit/quat_norm_gold.bin", "rb");

    if (file == NULL || fread(&expected, 1u, sizeof(expected), file) != sizeof(expected) ||
        fclose(file) != 0) {
        (void)puts("cannot load quat_norm original-ELF gold");
        return 1;
    }
    memcpy(preserved_xyz, xyz, sizeof(xyz));
    actual = quat_norm(&quat);
    if (memcmp(&actual, &expected, sizeof(actual)) != 0 ||
        memcmp(xyz, preserved_xyz, sizeof(xyz)) != 0) {
        (void)puts("quat_norm original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: quat_norm controlled original-ELF gold compare");
    return 0;
}

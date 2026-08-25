#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

static int load_gold(const char *path, double output[9])
{
    FILE *file = fopen(path, "rb");
    size_t count;

    if (file == NULL) {
        (void)fprintf(stderr, "cannot open gold: %s\n", path);
        return 1;
    }
    count = fread(output, 1u, 9u * sizeof(double), file);
    if (fclose(file) != 0 || count != 9u * sizeof(double)) {
        (void)fprintf(stderr, "invalid gold: %s\n", path);
        return 1;
    }
    return 0;
}

int main(void)
{
    double y_actual[9];
    double z_actual[9];
    double y_expected[9];
    double z_expected[9];
    DpMatrix y = {3, 3, 3, 0, y_actual};
    DpMatrix z = {3, 3, 3, 0, z_actual};

    if (load_gold("analysis/time_orbit/rotationy_gold.bin", y_expected) != 0 ||
        load_gold("analysis/time_orbit/rotationz_gold.bin", z_expected) != 0) {
        return 1;
    }
    memset(y_actual, 0xa5, sizeof(y_actual));
    memset(z_actual, 0xa5, sizeof(z_actual));
    Rotation_Y(&y, 1.1);
    Rotation_Z(&z, -0.7);
    if (memcmp(y_actual, y_expected, sizeof(y_actual)) != 0 ||
        memcmp(z_actual, z_expected, sizeof(z_actual)) != 0 ||
        y.rows != 3 || y.cols != 3 || y.row_stride != 3 ||
        z.rows != 3 || z.cols != 3 || z.row_stride != 3) {
        (void)puts("Rotation_Y/Rotation_Z original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: Rotation_Y / Rotation_Z controlled original-ELF gold compare");
    return 0;
}

#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

static int load_gold(const char *path, void *destination, size_t bytes)
{
    FILE *file = fopen(path, "rb");
    size_t actual;

    if (file == NULL) {
        (void)fprintf(stderr, "cannot open gold: %s\n", path);
        return 1;
    }
    actual = fread(destination, 1u, bytes, file);
    if (fclose(file) != 0 || actual != bytes) {
        (void)fprintf(stderr, "invalid gold length: %s\n", path);
        return 1;
    }
    return 0;
}

int main(void)
{
    double input_xyz[3] = {-2.5, 0.0, -0.0};
    double output_xyz[3] = {0.0, 0.0, 0.0};
    double expected_w;
    double expected_xyz[3];
    double preserved_xyz[3];
    DpQuatAbi input = {1.25, {3, 0, input_xyz}};
    DpQuatAbi output = {0.0, {3, 0, output_xyz}};

    if (load_gold("analysis/time_orbit/quat_conjugate_gold_w.bin", &expected_w,
                  sizeof(expected_w)) != 0 ||
        load_gold("analysis/time_orbit/quat_conjugate_gold_xyz.bin", expected_xyz,
                  sizeof(expected_xyz)) != 0) {
        return 1;
    }
    memcpy(preserved_xyz, input_xyz, sizeof(input_xyz));
    quat_conjugate(&output, &input);
    if (memcmp(&output.w, &expected_w, sizeof(output.w)) != 0 ||
        memcmp(output_xyz, expected_xyz, sizeof(output_xyz)) != 0 ||
        memcmp(&input.w, &(double){1.25}, sizeof(input.w)) != 0 ||
        memcmp(input_xyz, preserved_xyz, sizeof(input_xyz)) != 0) {
        (void)puts("quat_conjugate original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: quat_conjugate controlled original-ELF gold compare");
    return 0;
}

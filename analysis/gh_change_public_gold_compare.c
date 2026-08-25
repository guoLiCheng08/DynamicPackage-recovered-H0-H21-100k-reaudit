#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_environment.h"

#define DP_GH_MATRIX_BYTES (14u * 14u * sizeof(double))

static int load_gold(const char *path, double matrix[14][14])
{
    FILE *file = fopen(path, "rb");
    size_t bytes;

    if (file == NULL) {
        (void)fprintf(stderr, "cannot open gold: %s\n", path);
        return 1;
    }
    bytes = fread(matrix, 1u, DP_GH_MATRIX_BYTES, file);
    if (fclose(file) != 0 || bytes != DP_GH_MATRIX_BYTES) {
        (void)fprintf(stderr, "invalid gold length: %s\n", path);
        return 1;
    }
    return 0;
}

int main(void)
{
    double expected_g[14][14];
    double expected_h[14][14];
    double actual_g[14][14];
    double actual_h[14][14];

    if (load_gold("analysis/time_orbit/gh_change_gold_g.bin", expected_g) != 0 ||
        load_gold("analysis/time_orbit/gh_change_gold_h.bin", expected_h) != 0) {
        return 1;
    }
    memset(actual_g, 0xa5, sizeof(actual_g));
    memset(actual_h, 0xa5, sizeof(actual_h));
    gh_change(actual_g, actual_h, 2027.25);
    if (memcmp(actual_g, expected_g, sizeof(actual_g)) != 0 ||
        memcmp(actual_h, expected_h, sizeof(actual_h)) != 0) {
        (void)puts("gh_change complete 14x14 coefficient output mismatch");
        return 1;
    }
    (void)puts("PASS: gh_change complete controlled original-ELF gold compare");
    return 0;
}

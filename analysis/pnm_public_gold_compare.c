#include <stdio.h>
#include <string.h>

#include "dynamic_environment.h"

#define DP_PNM_MATRIX_BYTES (14u * 14u * sizeof(double))

static int load_gold(const char *path, double matrix[14][14])
{
    FILE *file = fopen(path, "rb");
    size_t bytes;

    if (file == NULL) {
        (void)fprintf(stderr, "cannot open gold: %s\n", path);
        return 1;
    }
    bytes = fread(matrix, 1u, DP_PNM_MATRIX_BYTES, file);
    if (fclose(file) != 0 || bytes != DP_PNM_MATRIX_BYTES) {
        (void)fprintf(stderr, "invalid gold length: %s\n", path);
        return 1;
    }
    return 0;
}

int main(void)
{
    double expected_p[14][14];
    double expected_derivative[14][14];
    double actual_p[14][14];
    double actual_derivative[14][14];

    if (load_gold("analysis/time_orbit/pnm_gold_p.bin", expected_p) != 0 ||
        load_gold("analysis/time_orbit/pnm_gold_d.bin", expected_derivative) != 0) {
        return 1;
    }
    memset(actual_p, 0xa5, sizeof(actual_p));
    memset(actual_derivative, 0xa5, sizeof(actual_derivative));
    Pnm(actual_p, actual_derivative, 1.1);
    if (memcmp(actual_p, expected_p, sizeof(actual_p)) != 0 ||
        memcmp(actual_derivative, expected_derivative, sizeof(actual_derivative)) != 0) {
        (void)puts("Pnm complete 14x14 P/derivative output mismatch");
        return 1;
    }
    (void)puts("PASS: Pnm complete controlled original-ELF gold compare");
    return 0;
}

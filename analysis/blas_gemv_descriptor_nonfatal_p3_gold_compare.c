#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_math.h"

#define GOLD_DIR "analysis/time_orbit/"
#define RESULT_BYTES 20u
#define SNAPSHOT_COUNT 8u

static int read_exact(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    size_t count;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    if (fclose(file) != 0 || count != bytes) {
        return -1;
    }
    return 0;
}

static int compare_blob(const char *label, const unsigned char *actual,
                        const unsigned char *expected, size_t bytes)
{
    size_t index;
    unsigned mismatches = 0u;

    for (index = 0u; index < bytes; ++index) {
        if (actual[index] != expected[index]) {
            if (mismatches < 8u) {
                printf("%s byte+0x%zx actual=%02x expected=%02x\n", label, index,
                       (unsigned)actual[index], (unsigned)expected[index]);
            }
            ++mismatches;
        }
    }
    printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

int main(void)
{
    unsigned char actual_result[RESULT_BYTES] = {0};
    unsigned char expected_result[RESULT_BYTES];
    double expected_snapshots[SNAPSHOT_COUNT];
    double matrix_data[6] = {1.0, 2.0, 99.0, 3.0, 4.0, 99.0};
    double x_data[2] = {2.0, -1.0};
    double y_data[2] = {0.0, 0.0};
    double snapshots[SNAPSHOT_COUNT];
    DpMatrix matrix = {2, 2, 3, 0, matrix_data};
    DpVector x = {1, 0, x_data};
    DpVector y = {1, 0, y_data};
    int return_code;
    int mismatch = 0;

    if (read_exact("gold_blas_gemv_descriptor_nonfatal_p3_result.bin", expected_result,
                   sizeof(expected_result)) != 0 ||
        read_exact("gold_blas_gemv_descriptor_nonfatal_p3_snapshots.bin", expected_snapshots,
                   sizeof(expected_snapshots)) != 0) {
        return 1;
    }

    y_data[0] = 10.0;
    y_data[1] = -3.0;
    return_code = blas_gemv(&matrix, &x, &y, 2.0, 0.5);
    memcpy(actual_result + 0u, &return_code, sizeof(return_code));
    snapshots[0] = y_data[0];
    snapshots[1] = y_data[1];

    y.count = 2;
    return_code = blas_gemv(&matrix, &x, &y, 0.0, -2.0);
    memcpy(actual_result + 4u, &return_code, sizeof(return_code));
    snapshots[2] = y_data[0];
    snapshots[3] = y_data[1];

    return_code = blas_gemv(&matrix, &x, &y, 2.0, 0.5);
    memcpy(actual_result + 8u, &return_code, sizeof(return_code));
    snapshots[4] = y_data[0];
    snapshots[5] = y_data[1];

    matrix.cols = 0;
    x.data = NULL;
    y_data[0] = 8.0;
    y_data[1] = -4.0;
    return_code = blas_gemv(&matrix, &x, &y, 2.0, 0.5);
    memcpy(actual_result + 12u, &return_code, sizeof(return_code));
    snapshots[6] = y_data[0];
    snapshots[7] = y_data[1];

    matrix.rows = 0;
    y.count = 0;
    return_code = blas_gemv(&matrix, &x, &y, 2.0, 0.5);
    memcpy(actual_result + 16u, &return_code, sizeof(return_code));

    mismatch |= compare_blob("blas_gemv P3 returns", actual_result, expected_result,
                             sizeof(actual_result));
    mismatch |= compare_blob("blas_gemv P3 snapshots",
                             (const unsigned char *)snapshots,
                             (const unsigned char *)expected_snapshots,
                             sizeof(snapshots));
    if (mismatch == 0) {
        puts("blas_gemv nonfatal P3 original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

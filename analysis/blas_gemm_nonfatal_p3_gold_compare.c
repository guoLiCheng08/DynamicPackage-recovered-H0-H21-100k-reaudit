#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_exact(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    size_t count;
    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) { perror(path); return -1; }
    count = fread(out, 1u, bytes, file);
    return fclose(file) == 0 && count == bytes ? 0 : -1;
}

static int compare_blob(const char *label, const unsigned char *actual,
                        const unsigned char *expected, size_t bytes)
{
    size_t index;
    unsigned mismatches = 0u;
    for (index = 0u; index < bytes; ++index) {
        if (actual[index] != expected[index]) ++mismatches;
    }
    printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

int main(void)
{
    unsigned char expected_results[12];
    unsigned char actual_results[12] = {0};
    double expected_snapshots[3];
    double expected_out[2];
    double left_data[2] = {2.0, 3.0};
    double right_data[2] = {4.0, 5.0};
    double out_data[2] = {7.0, 11.0};
    DpMatrix left = {1, 2, 2, 0, left_data};
    DpMatrix right = {2, 1, 1, 0, right_data};
    DpMatrix out = {1, 1, 1, 0, out_data};
    double actual_snapshots[3];
    int result;
    int mismatch = 0;

    if (read_exact("gold_blas_gemm_nonfatal_p3_results.bin", expected_results,
                   sizeof(expected_results)) != 0 ||
        read_exact("gold_blas_gemm_nonfatal_p3_snapshots.bin", expected_snapshots,
                   sizeof(expected_snapshots)) != 0 ||
        read_exact("gold_blas_gemm_nonfatal_p3_out.bin", expected_out,
                   sizeof(expected_out)) != 0) return 1;
    result = blas_gemm(&left, &right, &out, 0.0, 2.0);
    memcpy(actual_results, &result, sizeof(result));
    actual_snapshots[0] = out_data[0];
    result = blas_gemm(&left, &right, &out, 1.0, 2.0);
    memcpy(actual_results + 4u, &result, sizeof(result));
    actual_snapshots[1] = out_data[0];
    out.rows = 2;
    result = blas_gemm(&left, &right, &out, 1.0, 2.0);
    memcpy(actual_results + 8u, &result, sizeof(result));
    actual_snapshots[2] = out_data[0];
    mismatch |= compare_blob("blas_gemm P3 returns", actual_results, expected_results,
                             sizeof(actual_results));
    mismatch |= compare_blob("blas_gemm P3 snapshots", (const unsigned char *)actual_snapshots,
                             (const unsigned char *)expected_snapshots, sizeof(actual_snapshots));
    mismatch |= compare_blob("blas_gemm P3 output", (const unsigned char *)out_data,
                             (const unsigned char *)expected_out, sizeof(out_data));
    if (mismatch == 0) puts("blas_gemm nonfatal P3 original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

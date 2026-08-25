#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_exact(const char *name, void *out, size_t bytes)
{
    char path[256]; FILE *file; size_t count;
    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) { perror(path); return -1; }
    count = fread(out, 1u, bytes, file);
    return fclose(file) == 0 && count == bytes ? 0 : -1;
}

static int compare_blob(const char *label, const unsigned char *actual,
                        const unsigned char *expected, size_t bytes)
{
    size_t index; unsigned mismatches = 0u;
    for (index = 0u; index < bytes; ++index) if (actual[index] != expected[index]) ++mismatches;
    printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

int main(void)
{
    unsigned char expected_results[8];
    unsigned char actual_results[8] = {0};
    double expected_left[8];
    double left_data[8] = {1.0, 2.0, 90.0, 3.0, 4.0, 91.0, 0.0, 0.0};
    double right_data[10] = {10.0,20.0,30.0,40.0,50.0,60.0,70.0,80.0,0.0,0.0};
    DpMatrix left = {2, 2, 3, 0, left_data};
    DpMatrix right = {3, 2, 4, 0, right_data};
    int result;
    int mismatch = 0;

    if (read_exact("gold_matrix_add_nonfatal_p3_results.bin", expected_results,
                   sizeof(expected_results)) != 0 ||
        read_exact("gold_matrix_add_nonfatal_p3_left.bin", expected_left,
                   sizeof(expected_left)) != 0) return 1;
    result = matrix_add(&left, &right);
    memcpy(actual_results, &result, sizeof(result));
    right.rows = 2;
    result = matrix_add(&left, &right);
    memcpy(actual_results + 4u, &result, sizeof(result));
    mismatch |= compare_blob("matrix_add P3 returns", actual_results, expected_results,
                             sizeof(actual_results));
    mismatch |= compare_blob("matrix_add P3 left", (const unsigned char *)left_data,
                             (const unsigned char *)expected_left, sizeof(left_data));
    if (mismatch == 0) puts("matrix_add nonfatal P3 original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

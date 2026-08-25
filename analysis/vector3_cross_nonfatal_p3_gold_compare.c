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
    if (file == NULL) {
        perror(path);
        return -1;
    }
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
    unsigned char expected_results[8];
    unsigned char actual_results[8] = {0};
    double expected_out[3];
    double left_data[3] = {1.0, 2.0, 3.0};
    double right_data[3] = {-4.0, 5.0, -6.0};
    double out_data[3] = {9.0, 8.0, 7.0};
    DpVector left = {2, 0, left_data};
    DpVector right = {3, 0, right_data};
    DpVector out = {1, 0, out_data};
    int result;
    int mismatch = 0;

    if (read_exact("gold_vector3_cross_nonfatal_p3_results.bin", expected_results,
                   sizeof(expected_results)) != 0 ||
        read_exact("gold_vector3_cross_nonfatal_p3_out.bin", expected_out,
                   sizeof(expected_out)) != 0) return 1;
    result = vector3_cross(&left, &right, &out);
    memcpy(actual_results, &result, sizeof(result));
    right.count = 2;
    result = vector3_cross(&left, &right, &out);
    memcpy(actual_results + 4u, &result, sizeof(result));
    mismatch |= compare_blob("vector3_cross P3 returns", actual_results, expected_results,
                             sizeof(actual_results));
    mismatch |= compare_blob("vector3_cross P3 output", (const unsigned char *)out_data,
                             (const unsigned char *)expected_out, sizeof(out_data));
    if (mismatch == 0) puts("vector3_cross nonfatal P3 original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

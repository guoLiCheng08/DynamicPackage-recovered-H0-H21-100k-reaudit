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
    unsigned char actual_results[12] = {0};
    unsigned char expected_results[12];
    double expected_left[3];
    double left_data[3] = {5.0, 3.0, -2.5};
    double right_data[3] = {1.0, -4.0, 0.5};
    DpVector left = {2, 0, left_data};
    DpVector right = {3, 0, right_data};
    int result;
    int mismatch = 0;

    if (read_exact("gold_vector_sub_nonfatal_p3_results.bin", expected_results,
                   sizeof(expected_results)) != 0 ||
        read_exact("gold_vector_sub_nonfatal_p3_left.bin", expected_left,
                   sizeof(expected_left)) != 0) return 1;
    result = vector_sub(&left, &right);
    memcpy(actual_results, &result, sizeof(result));
    left.count = 0;
    right.count = 0;
    result = vector_sub(&left, &right);
    memcpy(actual_results + 4u, &result, sizeof(result));
    left.count = 3;
    right.count = 3;
    result = vector_sub(&left, &right);
    memcpy(actual_results + 8u, &result, sizeof(result));
    mismatch |= compare_blob("vector_sub P3 returns", actual_results, expected_results,
                             sizeof(actual_results));
    mismatch |= compare_blob("vector_sub P3 left", (const unsigned char *)left_data,
                             (const unsigned char *)expected_left, sizeof(left_data));
    if (mismatch == 0) puts("vector_sub nonfatal P3 original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

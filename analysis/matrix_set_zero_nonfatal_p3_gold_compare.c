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
    unsigned char expected_result[4];
    unsigned char actual_result[4];
    double expected_data[12];
    double data[12];
    DpMatrix matrix = {2, 2, 4, 0, data};
    unsigned index;
    int result;
    int mismatch = 0;

    if (read_exact("gold_matrix_set_zero_nonfatal_p3_result.bin", expected_result,
                   sizeof(expected_result)) != 0 ||
        read_exact("gold_matrix_set_zero_nonfatal_p3_data.bin", expected_data,
                   sizeof(expected_data)) != 0) return 1;
    for (index = 0u; index < 12u; ++index) data[index] = (double)index + 1.0;
    result = matrix_set_zero(&matrix);
    memcpy(actual_result, &result, sizeof(result));
    mismatch |= compare_blob("matrix_set_zero P3 return", actual_result, expected_result,
                             sizeof(actual_result));
    mismatch |= compare_blob("matrix_set_zero P3 data", (const unsigned char *)data,
                             (const unsigned char *)expected_data, sizeof(data));
    if (mismatch == 0) puts("matrix_set_zero nonfatal P3 original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

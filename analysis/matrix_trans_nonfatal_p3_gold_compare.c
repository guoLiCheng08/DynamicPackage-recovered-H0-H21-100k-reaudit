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
    double expected_dst[12];
    double src_data[6] = {1.0, 2.0, 99.0, 3.0, 4.0, 99.0};
    double dst_data[12];
    DpMatrix src = {2, 2, 3, 0, src_data};
    DpMatrix dst = {1, 3, 4, 0, dst_data};
    unsigned index;
    int result;
    int mismatch = 0;

    if (read_exact("gold_matrix_trans_nonfatal_p3_result.bin", expected_result,
                   sizeof(expected_result)) != 0 ||
        read_exact("gold_matrix_trans_nonfatal_p3_dst.bin", expected_dst,
                   sizeof(expected_dst)) != 0) return 1;
    for (index = 0u; index < 12u; ++index) dst_data[index] = -1.0;
    result = matrix_trans(&dst, &src);
    memcpy(actual_result, &result, sizeof(result));
    mismatch |= compare_blob("matrix_trans P3 return", actual_result, expected_result,
                             sizeof(actual_result));
    mismatch |= compare_blob("matrix_trans P3 destination", (const unsigned char *)dst_data,
                             (const unsigned char *)expected_dst, sizeof(dst_data));
    if (mismatch == 0) puts("matrix_trans nonfatal P3 original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

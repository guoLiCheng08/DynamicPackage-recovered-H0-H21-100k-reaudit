#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_math.h"

#define GOLD_DIR "analysis/time_orbit/"
#define RESULTS_BYTES 0x30u
#define LHS_BYTES (3u * sizeof(double))

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
    unsigned char expected_results[RESULTS_BYTES];
    unsigned char actual_results[RESULTS_BYTES] = {0};
    double expected_lhs[3];
    double lhs_data[3] = {1.0, 2.0, 3.0};
    double rhs_short_data[3] = {10.0, 20.0, 30.0};
    double dot_out = 123.0;
    DpVector lhs = {3, 0, lhs_data};
    DpVector rhs_short = {2, 0, rhs_short_data};
    DpVector empty_left = {0, 0, NULL};
    DpVector empty_right = {0, 0, NULL};
    int return_code;
    int mismatch = 0;

    if (read_exact("gold_vector_descriptor_nonfatal_p3_results.bin", expected_results,
                   sizeof(expected_results)) != 0 ||
        read_exact("gold_vector_descriptor_nonfatal_p3_lhs_data.bin", expected_lhs,
                   sizeof(expected_lhs)) != 0) {
        return 1;
    }

    return_code = vector_add(&lhs, &rhs_short);
    memcpy(actual_results + 0x00u, &return_code, sizeof(return_code));
    return_code = vector_memcpy(&lhs, &rhs_short);
    memcpy(actual_results + 0x04u, &return_code, sizeof(return_code));
    return_code = vector_dot(&lhs, &rhs_short, &dot_out);
    memcpy(actual_results + 0x08u, &return_code, sizeof(return_code));
    memcpy(actual_results + 0x18u, &dot_out, sizeof(dot_out));

    dot_out = 456.0;
    return_code = vector_add(&empty_left, &empty_right);
    memcpy(actual_results + 0x0cu, &return_code, sizeof(return_code));
    return_code = vector_scale(&empty_left, 7.0);
    memcpy(actual_results + 0x10u, &return_code, sizeof(return_code));
    return_code = vector_dot(&empty_left, &empty_right, &dot_out);
    memcpy(actual_results + 0x14u, &return_code, sizeof(return_code));
    memcpy(actual_results + 0x20u, &dot_out, sizeof(dot_out));

    mismatch |= compare_blob("vector descriptor nonfatal results", actual_results,
                             expected_results, sizeof(actual_results));
    mismatch |= compare_blob("vector descriptor nonfatal lhs", (const unsigned char *)lhs_data,
                             (const unsigned char *)expected_lhs, LHS_BYTES);
    if (mismatch == 0) {
        puts("vector descriptor nonfatal P3 original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

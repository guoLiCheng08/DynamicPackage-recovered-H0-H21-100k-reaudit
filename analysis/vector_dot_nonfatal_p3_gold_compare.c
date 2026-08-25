#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

#define GOLD_DIR "analysis/time_orbit/"
#define RESULT_BYTES 12u
#define SNAPSHOT_BYTES 24u

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
    if (fclose(file) != 0 || count != bytes) return -1;
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
    unsigned char actual_results[RESULT_BYTES] = {0};
    unsigned char expected_results[RESULT_BYTES];
    double actual_snapshots[3];
    double expected_snapshots[3];
    double left_data[3] = {1.5, -2.0, 4.0};
    double right_data[3] = {-3.0, 0.25, 2.0};
    DpVector left = {2, 0, left_data};
    DpVector right = {3, 0, right_data};
    double out_dot = 123.5;
    int result;
    int mismatch = 0;

    if (read_exact("gold_vector_dot_nonfatal_p3_results.bin", expected_results,
                   sizeof(expected_results)) != 0 ||
        read_exact("gold_vector_dot_nonfatal_p3_snapshots.bin", expected_snapshots,
                   sizeof(expected_snapshots)) != 0) return 1;

    result = vector_dot(&left, &right, &out_dot);
    memcpy(actual_results, &result, sizeof(result));
    actual_snapshots[0] = out_dot;
    left.count = 0;
    right.count = 0;
    result = vector_dot(&left, &right, &out_dot);
    memcpy(actual_results + 4u, &result, sizeof(result));
    actual_snapshots[1] = out_dot;
    left.count = 3;
    right.count = 3;
    result = vector_dot(&left, &right, &out_dot);
    memcpy(actual_results + 8u, &result, sizeof(result));
    actual_snapshots[2] = out_dot;

    mismatch |= compare_blob("vector_dot P3 returns", actual_results, expected_results,
                             sizeof(actual_results));
    mismatch |= compare_blob("vector_dot P3 snapshots",
                             (const unsigned char *)actual_snapshots,
                             (const unsigned char *)expected_snapshots,
                             sizeof(actual_snapshots));
    if (mismatch == 0) puts("vector_dot nonfatal P3 original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

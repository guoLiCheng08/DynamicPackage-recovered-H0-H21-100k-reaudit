#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

static int read_blob(const char *path, void *out, size_t bytes)
{
    FILE *file = fopen(path, "rb");
    size_t count;

    if (file == NULL) {
        perror(path);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    if (fclose(file) != 0 || count != bytes) return -1;
    return 0;
}

static int compare_blob(const char *label, const void *actual, const void *expected,
                        size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;
    unsigned mismatches = 0u;

    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            if (mismatches < 8u) {
                printf("%s byte+0x%zx actual=%02x expected=%02x\n", label, index,
                       (unsigned)a[index], (unsigned)e[index]);
            }
            ++mismatches;
        }
    }
    printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

int main(void)
{
    int expected_result[3];
    double expected_left[3];
    int actual_result[3];
    double left_data[3] = {1.0, -2.0, 3.5};
    double right_data[3] = {4.0, 5.0, -6.0};
    DpVector left = {2, 0, left_data};
    DpVector right = {3, 0, right_data};
    int mismatch = 0;

    if (read_blob("analysis/time_orbit/gold_vector_add_nonfatal_p3_result.bin",
                  expected_result, sizeof(expected_result)) != 0 ||
        read_blob("analysis/time_orbit/gold_vector_add_nonfatal_p3_left.bin",
                  expected_left, sizeof(expected_left)) != 0) {
        return 1;
    }

    actual_result[0] = vector_add(&left, &right);
    left.count = 0;
    right.count = 0;
    actual_result[1] = vector_add(&left, &right);
    left.count = 3;
    right.count = 3;
    actual_result[2] = vector_add(&left, &right);

    mismatch |= compare_blob("vector_add P3 returns", actual_result, expected_result,
                             sizeof(actual_result));
    mismatch |= compare_blob("vector_add P3 left", left_data, expected_left,
                             sizeof(left_data));
    if (mismatch == 0) {
        puts("vector_add nonfatal P3 original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

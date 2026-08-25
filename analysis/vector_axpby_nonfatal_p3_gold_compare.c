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
    double expected_source[3];
    double expected_destination[3];
    int actual_result[3];
    double source_data[3] = {1.0, -2.0, 3.0};
    double destination_data[3] = {10.0, 20.0, -30.0};
    DpVector source = {3, 0, source_data};
    DpVector destination = {2, 0, destination_data};
    int mismatch = 0;

    if (read_blob("analysis/time_orbit/gold_vector_axpby_nonfatal_p3_result.bin",
                  expected_result, sizeof(expected_result)) != 0 ||
        read_blob("analysis/time_orbit/gold_vector_axpby_nonfatal_p3_source.bin",
                  expected_source, sizeof(expected_source)) != 0 ||
        read_blob("analysis/time_orbit/gold_vector_axpby_nonfatal_p3_destination.bin",
                  expected_destination, sizeof(expected_destination)) != 0) {
        return 1;
    }

    actual_result[0] = vector_axpby(&source, 0.0, &destination, -0.5);
    source.count = 0;
    actual_result[1] = vector_axpby(&source, 2.0, &destination, -0.5);
    source.count = 3;
    destination.count = 2;
    actual_result[2] = vector_axpby(&source, 2.0, &destination, -0.5);

    mismatch |= compare_blob("vector_axpby P3 returns", actual_result, expected_result,
                             sizeof(actual_result));
    mismatch |= compare_blob("vector_axpby P3 source", source_data, expected_source,
                             sizeof(source_data));
    mismatch |= compare_blob("vector_axpby P3 destination", destination_data,
                             expected_destination, sizeof(destination_data));
    if (mismatch == 0) {
        puts("vector_axpby nonfatal P3 original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

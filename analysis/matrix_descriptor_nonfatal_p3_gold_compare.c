#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_math.h"

#define GOLD_DIR "analysis/time_orbit/"
#define RESULT_BYTES 8u
#define DESTINATION_COUNT 12u

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
    unsigned char actual_result[RESULT_BYTES] = {0};
    unsigned char expected_result[RESULT_BYTES];
    double source_data[6] = {1.0, 2.0, 99.0, 3.0, 4.0, 99.0};
    double destination_data[DESTINATION_COUNT];
    double expected_destination[DESTINATION_COUNT];
    DpMatrix source = {2, 2, 3, 0, source_data};
    DpMatrix destination = {3, 1, 4, 0, destination_data};
    int return_code;
    unsigned index;
    int mismatch = 0;

    if (read_exact("gold_matrix_descriptor_nonfatal_p3_result.bin", expected_result,
                   sizeof(expected_result)) != 0 ||
        read_exact("gold_matrix_descriptor_nonfatal_p3_destination.bin", expected_destination,
                   sizeof(expected_destination)) != 0) {
        return 1;
    }
    for (index = 0u; index < DESTINATION_COUNT; ++index) {
        destination_data[index] = -1.0;
    }
    return_code = matrix_memcpy(&destination, &source);
    memcpy(actual_result, &return_code, sizeof(return_code));

    mismatch |= compare_blob("matrix descriptor nonfatal return", actual_result,
                             expected_result, sizeof(actual_result));
    mismatch |= compare_blob("matrix descriptor nonfatal destination",
                             (const unsigned char *)destination_data,
                             (const unsigned char *)expected_destination,
                             sizeof(destination_data));
    if (mismatch == 0) {
        puts("matrix descriptor nonfatal P3 original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

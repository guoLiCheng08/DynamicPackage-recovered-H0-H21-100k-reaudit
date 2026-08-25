#include <stdio.h>

#include "dynamic_math.h"

#define STDOUT_GOLD_PATH "analysis/time_orbit/gold_vector_print_p3_stdout_clean.txt"
#define RETURN_GOLD_PATH "analysis/time_orbit/gold_vector_print_p3_returns.bin"
#define ACTUAL_STDOUT_PATH "/tmp/vector_print_p3_recovered_stdout.txt"

int main(void)
{
    unsigned char expected_stdout[256];
    unsigned char actual_stdout[256];
    int expected_returns[2];
    int actual_returns[2];
    double data[3] = {1.0, -2.5, 3.25};
    DpVector vector = {0, 0, data};
    FILE *file;
    size_t expected_stdout_size;
    size_t actual_stdout_size;
    size_t expected_return_size;
    size_t index;
    unsigned mismatches = 0u;

    file = fopen(STDOUT_GOLD_PATH, "rb");
    if (file == NULL) { perror(STDOUT_GOLD_PATH); return 1; }
    expected_stdout_size = fread(expected_stdout, 1u, sizeof(expected_stdout), file);
    if (fclose(file) != 0) return 1;
    file = fopen(RETURN_GOLD_PATH, "rb");
    if (file == NULL) { perror(RETURN_GOLD_PATH); return 1; }
    expected_return_size = fread(expected_returns, 1u, sizeof(expected_returns), file);
    if (fclose(file) != 0 || expected_return_size != sizeof(expected_returns)) return 1;
    if (freopen(ACTUAL_STDOUT_PATH, "wb", stdout) == NULL) { perror(ACTUAL_STDOUT_PATH); return 1; }
    actual_returns[0] = vector_print(&vector);
    vector.count = 3;
    actual_returns[1] = vector_print(&vector);
    if (fflush(stdout) != 0) return 1;
    file = fopen(ACTUAL_STDOUT_PATH, "rb");
    if (file == NULL) return 1;
    actual_stdout_size = fread(actual_stdout, 1u, sizeof(actual_stdout), file);
    if (fclose(file) != 0) return 1;
    if (actual_stdout_size != expected_stdout_size) ++mismatches;
    for (index = 0u; index < actual_stdout_size && index < expected_stdout_size; ++index) {
        if (actual_stdout[index] != expected_stdout[index]) ++mismatches;
    }
    for (index = 0u; index < 2u; ++index) {
        if (actual_returns[index] != expected_returns[index]) ++mismatches;
    }
    fprintf(stderr, "vector_print P3 mismatches: %u; stdout actual=%zu expected=%zu\n",
            mismatches, actual_stdout_size, expected_stdout_size);
    if (mismatches == 0u) fprintf(stderr, "vector_print P3 original-ELF compare: PASS (bitwise)\n");
    return mismatches == 0u ? 0 : 1;
}

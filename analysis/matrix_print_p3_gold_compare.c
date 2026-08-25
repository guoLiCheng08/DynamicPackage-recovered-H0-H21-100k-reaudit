#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

#define GOLD_PATH "analysis/time_orbit/gold_matrix_print_p3_stdout_clean.txt"
#define ACTUAL_PATH "/tmp/matrix_print_p3_recovered_stdout.txt"

int main(void)
{
    unsigned char expected[256];
    unsigned char actual[256];
    double data[8] = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0};
    DpMatrix matrix = {2, 2, 4, 0, data};
    FILE *file;
    size_t expected_size;
    size_t actual_size;
    size_t index;
    unsigned mismatches = 0u;

    file = fopen(GOLD_PATH, "rb");
    if (file == NULL) { perror(GOLD_PATH); return 1; }
    expected_size = fread(expected, 1u, sizeof(expected), file);
    if (fclose(file) != 0) return 1;
    if (freopen(ACTUAL_PATH, "wb", stdout) == NULL) { perror(ACTUAL_PATH); return 1; }
    matrix_print(&matrix);
    if (fflush(stdout) != 0) return 1;
    file = fopen(ACTUAL_PATH, "rb");
    if (file == NULL) return 1;
    actual_size = fread(actual, 1u, sizeof(actual), file);
    if (fclose(file) != 0) return 1;
    if (actual_size != expected_size) ++mismatches;
    for (index = 0u; index < actual_size && index < expected_size; ++index) {
        if (actual[index] != expected[index]) ++mismatches;
    }
    fprintf(stderr, "matrix_print P3 stdout mismatches: %u; actual=%zu expected=%zu\n",
            mismatches, actual_size, expected_size);
    if (mismatches == 0u) fprintf(stderr, "matrix_print P3 original-ELF compare: PASS (bitwise)\n");
    return mismatches == 0u ? 0 : 1;
}

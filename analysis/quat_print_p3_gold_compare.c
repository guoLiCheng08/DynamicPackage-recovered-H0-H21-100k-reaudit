#include <stdio.h>

#include "dynamic_math.h"

#define STDOUT_GOLD_PATH "analysis/time_orbit/gold_quat_print_p3_stdout.txt"
#define RETURN_GOLD_PATH "analysis/time_orbit/gold_quat_print_p3_return.bin"
#define ACTUAL_STDOUT_PATH "/tmp/quat_print_p3_recovered_stdout.txt"

int main(void)
{
    unsigned char expected_stdout[256];
    unsigned char actual_stdout[256];
    int expected_return;
    int actual_return;
    double xyz[3] = {-2.5, 3.25, -4.5};
    DpQuatAbi quat = {1.25, {3, 0, xyz}};
    FILE *file;
    size_t expected_stdout_size;
    size_t actual_stdout_size;
    size_t count;
    size_t index;
    unsigned mismatches = 0u;

    file = fopen(STDOUT_GOLD_PATH, "rb");
    if (file == NULL) { perror(STDOUT_GOLD_PATH); return 1; }
    expected_stdout_size = fread(expected_stdout, 1u, sizeof(expected_stdout), file);
    if (fclose(file) != 0) return 1;
    file = fopen(RETURN_GOLD_PATH, "rb");
    if (file == NULL) { perror(RETURN_GOLD_PATH); return 1; }
    count = fread(&expected_return, 1u, sizeof(expected_return), file);
    if (fclose(file) != 0 || count != sizeof(expected_return)) return 1;
    if (freopen(ACTUAL_STDOUT_PATH, "wb", stdout) == NULL) { perror(ACTUAL_STDOUT_PATH); return 1; }
    actual_return = quat_print(&quat);
    if (fflush(stdout) != 0) return 1;
    file = fopen(ACTUAL_STDOUT_PATH, "rb");
    if (file == NULL) return 1;
    actual_stdout_size = fread(actual_stdout, 1u, sizeof(actual_stdout), file);
    if (fclose(file) != 0) return 1;
    if (actual_stdout_size != expected_stdout_size) ++mismatches;
    for (index = 0u; index < actual_stdout_size && index < expected_stdout_size; ++index) {
        if (actual_stdout[index] != expected_stdout[index]) ++mismatches;
    }
    if (actual_return != expected_return) ++mismatches;
    fprintf(stderr, "quat_print P3 mismatches: %u; stdout actual=%zu expected=%zu; return actual=%d expected=%d\n",
            mismatches, actual_stdout_size, expected_stdout_size, actual_return, expected_return);
    if (mismatches == 0u) fprintf(stderr, "quat_print P3 original-ELF compare: PASS (bitwise)\n");
    return mismatches == 0u ? 0 : 1;
}

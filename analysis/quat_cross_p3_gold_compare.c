#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

#define QGOLD "analysis/time_orbit/gold_quat_cross_p3_output.bin"
#define XGOLD "analysis/time_orbit/gold_quat_cross_p3_xyz.bin"

static int read_exact(const char *path, unsigned char *destination, size_t length)
{
    FILE *file = fopen(path, "rb");
    const size_t count = file != NULL ? fread(destination, 1u, length, file) : 0u;

    if (file == NULL) {
        perror(path);
        return 0;
    }
    if (fclose(file) != 0 || count != length) {
        fprintf(stderr, "%s: expected %zu bytes, received %zu\n", path, length, count);
        return 0;
    }
    return 1;
}

static int report_difference(const char *name, const unsigned char *actual,
                             const unsigned char *expected, size_t length)
{
    size_t index;

    for (index = 0u; index < length; ++index) {
        if (actual[index] != expected[index]) {
            fprintf(stderr, "%s mismatch +0x%zx actual=%02x expected=%02x\n", name,
                    index, actual[index], expected[index]);
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    double out_xyz[3] = {101.0, 102.0, 103.0};
    double left_xyz[3] = {1.25, -2.5, 3.75};
    double right_xyz[3] = {4.25, -5.5, 6.75};
    DpQuatAbi out = {99.0, {3, 0x11223344, out_xyz}};
    DpQuatAbi left = {0.5, {3, 0x55667788, left_xyz}};
    DpQuatAbi right = {-1.5, {3, (int32_t)0x99aabbccu, right_xyz}};
    unsigned char expected_quat[sizeof out];
    unsigned char expected_xyz[sizeof out_xyz];

    if (!read_exact(QGOLD, expected_quat, sizeof expected_quat) ||
        !read_exact(XGOLD, expected_xyz, sizeof expected_xyz)) {
        return 1;
    }

    quat_cross(&out, &left, &right);

    if (report_difference("quat descriptor", (const unsigned char *)&out,
                          expected_quat, 16u) != 0 ||
        out.xyz.data != out_xyz ||
        report_difference("quat xyz", (const unsigned char *)out_xyz,
                          expected_xyz, sizeof out_xyz) != 0) {
        if (out.xyz.data != out_xyz) {
            fputs("quat descriptor data pointer was modified\n", stderr);
        }
        return 1;
    }

    puts("quat_cross P3 original-ELF compare: PASS (bitwise)");
    return 0;
}

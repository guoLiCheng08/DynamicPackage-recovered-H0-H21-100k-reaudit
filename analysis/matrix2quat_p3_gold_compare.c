#include <stdio.h>
#include <string.h>
#include "dynamic_math.h"

static int load(const char *path, void *data, size_t bytes)
{
    FILE *file = fopen(path, "rb");
    size_t count;
    if (file == NULL) { perror(path); return -1; }
    count = fread(data, 1u, bytes, file);
    return fclose(file) != 0 || count != bytes ? -1 : 0;
}

static int check_one(const char *tag, const double matrix_data[9],
                     const unsigned char expected_quat[24],
                     const unsigned char expected_xyz[24])
{
    double xyz[3] = {0.0, 0.0, 0.0};
    DpQuatAbi quat = {0.0, {3, 0, xyz}};
    DpMatrix matrix = {3, 3, 3, 0, (double *)matrix_data};
    matrix2quat(&quat, &matrix);
    if (memcmp(&quat, expected_quat, 16u) != 0 ||
        memcmp(xyz, expected_xyz, sizeof(xyz)) != 0) {
        fprintf(stderr, "matrix2quat %s mismatch\n", tag);
        return 1;
    }
    return 0;
}

int main(void)
{
    static const double identity[9] = {1,0,0,0,1,0,0,0,1};
    static const double rx[9] = {1,0,0,0,-1,0,0,0,-1};
    static const double ry[9] = {-1,0,0,0,1,0,0,0,-1};
    static const double rz180[9] = {-1,0,0,0,-1,0,0,0,1};
    static const double rz90[9] = {0,-1,0,1,0,0,0,0,1};
    unsigned char identity_q[24], identity_x[24], z90_q[24], z90_x[24];
    unsigned char axis_q[72], axis_x[72];
    int fail;

    if (load("analysis/time_orbit/gold_matrix2quat_identity_p3_quat.bin", identity_q, 24u) ||
        load("analysis/time_orbit/gold_matrix2quat_identity_p3_xyz.bin", identity_x, 24u) ||
        load("analysis/time_orbit/gold_matrix2quat_z90_p3_quat.bin", z90_q, 24u) ||
        load("analysis/time_orbit/gold_matrix2quat_z90_p3_xyz.bin", z90_x, 24u) ||
        load("analysis/time_orbit/gold_matrix2quat_axis_p3_quat.bin", axis_q, 72u) ||
        load("analysis/time_orbit/gold_matrix2quat_axis_p3_xyz.bin", axis_x, 72u)) return 1;
    fail = check_one("identity", identity, identity_q, identity_x);
    fail |= check_one("x-180", rx, axis_q, axis_x);
    fail |= check_one("y-180", ry, axis_q + 24, axis_x + 24);
    fail |= check_one("z-180", rz180, axis_q + 48, axis_x + 48);
    fail |= check_one("z-90", rz90, z90_q, z90_x);
    if (fail != 0) return 1;
    puts("matrix2quat P3 original-ELF compare: PASS (bitwise)");
    return 0;
}

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

static int read_gold(const char *path, void *out, size_t bytes)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return -1;
    if (fread(out, 1u, bytes, file) != bytes || fgetc(file) != EOF) {
        (void)fclose(file);
        return -1;
    }
    return fclose(file) == 0 ? 0 : -1;
}

int main(void)
{
    double attitude_xyz[3] = {0.0, 0.0, 0.0};
    double angular_rate[3] = {0.0, 0.0, 0.0};
    double inertial_to_body[9] = {0.0};
    double body_to_inertial[9] = {0.0};
    DpQuatAbi attitude = {0.0, {3, 0, attitude_xyz}};
    DpVector rate = {3, 0, angular_rate};
    DpMatrix inertial_to_body_matrix = {3, 3, 3, 0, inertial_to_body};
    DpMatrix body_to_inertial_matrix = {3, 3, 3, 0, body_to_inertial};
    static const uint64_t expected_attitude[4] = {
        UINT64_C(0x3fe0000000000000), UINT64_C(0xbfe0000000000000),
        UINT64_C(0x3fe0000000000000), UINT64_C(0xbfe0000000000000)
    };
    static const uint64_t expected_rate[3] = {
        UINT64_C(0x3ff4000000000000), UINT64_C(0xc004000000000000),
        UINT64_C(0x400e000000000000)
    };
    static const uint64_t expected_inertial_to_body[9] = {
        UINT64_C(0x0000000000000000), UINT64_C(0xbff0000000000000),
        UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000),
        UINT64_C(0x0000000000000000), UINT64_C(0xbff0000000000000),
        UINT64_C(0x3ff0000000000000), UINT64_C(0x0000000000000000),
        UINT64_C(0x0000000000000000)
    };
    static const uint64_t expected_body_to_inertial[9] = {
        UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000),
        UINT64_C(0x3ff0000000000000), UINT64_C(0xbff0000000000000),
        UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000),
        UINT64_C(0x0000000000000000), UINT64_C(0xbff0000000000000),
        UINT64_C(0x0000000000000000)
    };
    uint64_t actual_attitude[4];
    unsigned char expected_attitude_gold[7u * sizeof(double)];
    unsigned char expected_inertial_to_body_gold[sizeof(inertial_to_body)];
    unsigned char expected_body_to_inertial_gold[sizeof(body_to_inertial)];

    if (read_gold("analysis/time_orbit/get_attitude_gold.bin", expected_attitude_gold,
                  sizeof(expected_attitude_gold)) != 0 ||
        read_gold("analysis/time_orbit/get_inertial2body_gold.bin",
                  expected_inertial_to_body_gold, sizeof(expected_inertial_to_body_gold)) != 0 ||
        read_gold("analysis/time_orbit/get_body2inertial_gold.bin",
                  expected_body_to_inertial_gold, sizeof(expected_body_to_inertial_gold)) != 0) {
        return 2;
    }
    y[0] = 0.5;
    y[1] = -0.5;
    y[2] = 0.5;
    y[3] = -0.5;
    y[4] = 1.25;
    y[5] = -2.5;
    y[6] = 3.75;
    get_attitude(&attitude, &rate);
    get_Inertial2Body(&inertial_to_body_matrix);
    get_Body2Inertial(&body_to_inertial_matrix);
    memcpy(&actual_attitude[0], &attitude.w, sizeof(actual_attitude[0]));
    memcpy(&actual_attitude[1], attitude.xyz.data, sizeof(attitude_xyz));

    if (memcmp(actual_attitude, expected_attitude, sizeof(actual_attitude)) != 0 ||
        memcmp(angular_rate, expected_rate, sizeof(angular_rate)) != 0 ||
        memcmp(inertial_to_body, expected_inertial_to_body, sizeof(inertial_to_body)) != 0 ||
        memcmp(body_to_inertial, expected_body_to_inertial, sizeof(body_to_inertial)) != 0 ||
        memcmp(actual_attitude, expected_attitude_gold, sizeof(actual_attitude)) != 0 ||
        memcmp(angular_rate, expected_attitude_gold + sizeof(actual_attitude), sizeof(angular_rate)) != 0 ||
        memcmp(inertial_to_body, expected_inertial_to_body_gold, sizeof(inertial_to_body)) != 0 ||
        memcmp(body_to_inertial, expected_body_to_inertial_gold, sizeof(body_to_inertial)) != 0 ||
        attitude.xyz.count != 3 || attitude.xyz.reserved_04 != 0 ||
        attitude.xyz.data != attitude_xyz || rate.count != 3 || rate.reserved_04 != 0 ||
        rate.data != angular_rate || inertial_to_body_matrix.rows != 3 ||
        inertial_to_body_matrix.cols != 3 || inertial_to_body_matrix.row_stride != 3 ||
        inertial_to_body_matrix.data != inertial_to_body || body_to_inertial_matrix.rows != 3 ||
        body_to_inertial_matrix.cols != 3 || body_to_inertial_matrix.row_stride != 3 ||
        body_to_inertial_matrix.data != body_to_inertial) {
        (void)puts("attitude P1 controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: get_attitude/get_Inertial2Body/get_Body2Inertial controlled original-ELF gold compare");
    return 0;
}

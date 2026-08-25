#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_sensors.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    size_t read_count;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return 1;
    }
    read_count = fread(out, 1u, bytes, file);
    if (fclose(file) != 0 || read_count != bytes) {
        fprintf(stderr, "cannot read complete gold blob %s\n", path);
        return 1;
    }
    return 0;
}

static int compare_bytes(const char *label, const void *actual, const void *expected, size_t bytes)
{
    if (memcmp(actual, expected, bytes) != 0) {
        const uint8_t *actual_bytes = actual;
        const uint8_t *expected_bytes = expected;
        size_t index;

        for (index = 0u; index < bytes; ++index) {
            if (actual_bytes[index] != expected_bytes[index]) {
                fprintf(stderr, "%s mismatch at +0x%zx: got %02x expected %02x\n", label, index,
                        (unsigned)actual_bytes[index], (unsigned)expected_bytes[index]);
                break;
            }
        }
        return 1;
    }
    return 0;
}

static void relocate_devices(DpDeviceMeasureRecovered *devices)
{
    unsigned index;

    for (index = 0u; index < 2u; ++index) {
        const size_t item = (size_t)index * 0x320u;
        devices->gyro[index].projection_matrix.data = (double *)((uint8_t *)devices->gyro + item + 0xc8u);
        devices->gyro[index].measure.data = (double *)((uint8_t *)devices->gyro + item + 0x128u);
    }
    for (index = 0u; index < 2u; ++index) {
        const size_t item = (size_t)index * 0x138u;
        devices->magmeter[index].projection_matrix.data = (double *)((uint8_t *)devices->magmeter + item + 0xc0u);
        devices->magmeter[index].measure.data = (double *)((uint8_t *)devices->magmeter + item + 0x120u);
    }
    for (index = 0u; index < 3u; ++index) {
        const size_t item = (size_t)index * 0x170u;
        devices->sts[index].installation_matrix.data = (double *)((uint8_t *)devices->sts + item + 0x78u);
        devices->sts[index].error_quat.xyz.data = (double *)((uint8_t *)devices->sts + item + 0x118u);
        devices->sts[index].measure_quat.xyz.data = (double *)((uint8_t *)devices->sts + item + 0x158u);
    }
    for (index = 0u; index < 2u; ++index) {
        const size_t item = (size_t)index * 0x158u;
        devices->dss[index].projection_matrix.data = (double *)((uint8_t *)devices->dss + item + 0xb8u);
        devices->dss[index].measure.data = (double *)((uint8_t *)devices->dss + item + 0x120u);
    }
}

int main(void)
{
    DpDeviceMeasureRecovered devices;
    DpState state;
    DpDeviceMeasureEnvironment environment;
    uint8_t expected_gyro[0x640];
    uint8_t expected_magmeter[0x270];
    uint8_t expected_sts[0x450];
    uint8_t expected_dss[0x2b0];
    uint8_t expected_gps[0x70];
    unsigned index;

    memset(&devices, 0, sizeof(devices));
    memset(&state, 0, sizeof(state));
    memset(&environment, 0, sizeof(environment));
    if (read_blob("gold_sensor_init_gyro.bin", devices.gyro, sizeof(devices.gyro)) != 0 ||
        read_blob("gold_sensor_init_magmeter.bin", devices.magmeter, sizeof(devices.magmeter)) != 0 ||
        read_blob("gold_sensor_init_sts.bin", devices.sts, sizeof(devices.sts)) != 0 ||
        read_blob("gold_sensor_init_dss.bin", devices.dss, sizeof(devices.dss)) != 0 ||
        read_blob("gold_sensor_after_gyro.bin", expected_gyro, sizeof(expected_gyro)) != 0 ||
        read_blob("gold_sensor_after_magmeter.bin", expected_magmeter, sizeof(expected_magmeter)) != 0 ||
        read_blob("gold_sensor_after_sts.bin", expected_sts, sizeof(expected_sts)) != 0 ||
        read_blob("gold_sensor_after_dss.bin", expected_dss, sizeof(expected_dss)) != 0 ||
        read_blob("gold_sensor_after_gps.bin", expected_gps, sizeof(expected_gps)) != 0) {
        return 1;
    }
    relocate_devices(&devices);
    state.attitude_q.q0 = 1.0;
    state.body_rate.x = 0.01;
    state.body_rate.y = -0.02;
    state.body_rate.z = 0.03;
    state.position_gci.x = 7000000.0;
    state.position_gci.y = 1000.0;
    state.position_gci.z = -500.0;
    state.velocity_gci.x = 1.5;
    state.velocity_gci.y = -2.25;
    state.velocity_gci.z = 3.75;
    environment.sun_body.x = 0.2;
    environment.sun_body.y = -0.4;
    environment.sun_body.z = 0.8;
    environment.sun_gci.x = -1.0;
    environment.magnetic_body.x = 20.0;
    environment.magnetic_body.y = -30.0;
    environment.magnetic_body.z = 40.0;
    environment.time_values[0] = 2025.9;
    environment.time_values[1] = 1.1;
    environment.time_values[2] = 2.2;
    environment.time_values[3] = 3.3;
    environment.time_values[4] = 4.4;
    environment.time_values[5] = 5.5;

    dp_update_device_measure_recovered(&devices, &state, &environment, 0.1, NULL, NULL);

    for (index = 0u; index < 2u; ++index) {
        const size_t gyro_item = (size_t)index * 0x320u;
        const size_t magmeter_item = (size_t)index * 0x138u;
        char label[64];

        (void)snprintf(label, sizeof(label), "Gyro[%u] measure", index);
        if (compare_bytes(label, devices.gyro[index].measure.data, expected_gyro + gyro_item + 0x128u, 24u) != 0) {
            return 1;
        }
        (void)snprintf(label, sizeof(label), "MagMeter[%u] measure", index);
        if (compare_bytes(label, devices.magmeter[index].measure.data,
                          expected_magmeter + magmeter_item + 0x120u, 24u) != 0) {
            return 1;
        }
    }
    for (index = 0u; index < 3u; ++index) {
        const size_t item = (size_t)index * 0x170u;
        char label[64];

        (void)snprintf(label, sizeof(label), "STS[%u] valid", index);
        if (compare_bytes(label, &devices.sts[index].valid_flag, expected_sts + item + 0x138u, 4u) != 0) {
            return 1;
        }
        (void)snprintf(label, sizeof(label), "STS[%u] measure quaternion", index);
        if (compare_bytes(label, &devices.sts[index].measure_quat.w, expected_sts + item + 0x140u, 8u) != 0 ||
            compare_bytes(label, devices.sts[index].measure_quat.xyz.data, expected_sts + item + 0x158u, 24u) != 0) {
            return 1;
        }
    }
    for (index = 0u; index < 2u; ++index) {
        const size_t item = (size_t)index * 0x158u;
        char label[64];

        (void)snprintf(label, sizeof(label), "DSS[%u] measure", index);
        if (compare_bytes(label, devices.dss[index].measure.data, expected_dss + item + 0x120u, 24u) != 0 ||
            compare_bytes("DSS derived values", &devices.dss[index].angle_x, expected_dss + item + 0x138u, 32u) != 0 ||
            compare_bytes("DSS valid", &devices.dss[index].valid_flag, expected_dss + item + 0x108u, 4u) != 0) {
            return 1;
        }
    }
    if (compare_bytes("GPS position velocity", devices.gps.position_gci, expected_gps + 0x28u, 48u) != 0 ||
        compare_bytes("GPS time", devices.gps.time_components, expected_gps + 0x58u, 24u) != 0) {
        return 1;
    }
    puts("UpdateDeviceMeasure recovered full-chain original-ELF compare: PASS (all sensor outputs bitwise)");
    return 0;
}

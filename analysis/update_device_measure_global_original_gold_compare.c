#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"
#include "dynamic_satellite_globals.h"

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
        return -1;
    }
    read_count = fread(out, 1u, bytes, file);
    if (fclose(file) != 0 || read_count != bytes) {
        return -1;
    }
    return 0;
}

static int compare_bytes(const char *label, const void *actual, const void *expected,
                         size_t bytes)
{
    if (memcmp(actual, expected, bytes) != 0) {
        const unsigned char *a = actual;
        const unsigned char *e = expected;
        size_t index;
        for (index = 0u; index < bytes; ++index) {
            if (a[index] != e[index]) {
                printf("%s mismatch at +0x%zx: actual=%02x expected=%02x\n", label,
                       index, (unsigned)a[index], (unsigned)e[index]);
                break;
            }
        }
        return -1;
    }
    printf("%s mismatched bytes: 0/%zu\n", label, bytes);
    return 0;
}

int main(void)
{
    unsigned char core_output[0x148] = {0};
    uint8_t expected_gyro[0x640];
    uint8_t expected_magmeter[0x270];
    uint8_t expected_sts[0x450];
    uint8_t expected_dss[0x2b0];
    uint8_t expected_gps[0x70];
    DpQuat attitude = {1.0, 0.0, 0.0, 0.0};
    DpVec3 body_rate = {0.01, -0.02, 0.03};
    DpVec3 position = {7000000.0, 1000.0, -500.0};
    DpVec3 velocity = {1.5, -2.25, 3.75};
    DpVec3 sun_body = {0.2, -0.4, 0.8};
    DpVec3 sun_gci = {-1.0, 0.0, 0.0};
    DpVec3 magnetic_body = {20.0, -30.0, 40.0};
    double time_values[6] = {2025.9, 1.1, 2.2, 3.3, 4.4, 5.5};
    unsigned index;
    int mismatch = 0;

    if (read_blob("gold_sensor_init_gyro.bin", DeviceMeasure.gyro,
                  sizeof(DeviceMeasure.gyro)) != 0 ||
        read_blob("gold_sensor_init_magmeter.bin", DeviceMeasure.magmeter,
                  sizeof(DeviceMeasure.magmeter)) != 0 ||
        read_blob("gold_sensor_init_sts.bin", DeviceMeasure.sts,
                  sizeof(DeviceMeasure.sts)) != 0 ||
        read_blob("gold_sensor_init_dss.bin", DeviceMeasure.dss,
                  sizeof(DeviceMeasure.dss)) != 0 ||
        read_blob("gold_sensor_after_gyro.bin", expected_gyro, sizeof(expected_gyro)) != 0 ||
        read_blob("gold_sensor_after_magmeter.bin", expected_magmeter,
                  sizeof(expected_magmeter)) != 0 ||
        read_blob("gold_sensor_after_sts.bin", expected_sts, sizeof(expected_sts)) != 0 ||
        read_blob("gold_sensor_after_dss.bin", expected_dss, sizeof(expected_dss)) != 0 ||
        read_blob("gold_sensor_after_gps.bin", expected_gps, sizeof(expected_gps)) != 0) {
        return 1;
    }
    DeviceMeasure.gps_init_flag = 0;
    dp_device_measure_globals_relocate();

    memcpy(core_output + 0x000u, &attitude, sizeof(attitude));
    memcpy(core_output + 0x020u, &body_rate, sizeof(body_rate));
    memcpy(core_output + 0x038u, &sun_body, sizeof(sun_body));
    memcpy(core_output + 0x050u, &sun_gci, sizeof(sun_gci));
    memcpy(core_output + 0x068u, &magnetic_body, sizeof(magnetic_body));
    memcpy(core_output + 0x098u, &position, sizeof(position));
    memcpy(core_output + 0x0b0u, &velocity, sizeof(velocity));
    memcpy(core_output + 0x0c8u, time_values, sizeof(time_values));
    step_time = 0.1;

    UpdateDeviceMeasure(core_output);

    for (index = 0u; index < 2u; ++index) {
        const size_t gyro_item = (size_t)index * 0x320u;
        const size_t magmeter_item = (size_t)index * 0x138u;
        char label[64];
        (void)snprintf(label, sizeof(label), "Gyro[%u].measure", index);
        mismatch |= compare_bytes(label, DeviceMeasure.gyro[index].measure.data,
                                  expected_gyro + gyro_item + 0x128u, 24u);
        (void)snprintf(label, sizeof(label), "MagMeter[%u].measure", index);
        mismatch |= compare_bytes(label, DeviceMeasure.magmeter[index].measure.data,
                                  expected_magmeter + magmeter_item + 0x120u, 24u);
    }
    for (index = 0u; index < 3u; ++index) {
        const size_t item = (size_t)index * 0x170u;
        char label[64];
        (void)snprintf(label, sizeof(label), "STS[%u].valid", index);
        mismatch |= compare_bytes(label, &DeviceMeasure.sts[index].valid_flag,
                                  expected_sts + item + 0x138u, 4u);
        (void)snprintf(label, sizeof(label), "STS[%u].measure", index);
        mismatch |= compare_bytes(label, &DeviceMeasure.sts[index].measure_quat.w,
                                  expected_sts + item + 0x140u, 8u);
        mismatch |= compare_bytes(label, DeviceMeasure.sts[index].measure_quat.xyz.data,
                                  expected_sts + item + 0x158u, 24u);
    }
    for (index = 0u; index < 2u; ++index) {
        const size_t item = (size_t)index * 0x158u;
        char label[64];
        (void)snprintf(label, sizeof(label), "DSS[%u].measure", index);
        mismatch |= compare_bytes(label, DeviceMeasure.dss[index].measure.data,
                                  expected_dss + item + 0x120u, 24u);
        (void)snprintf(label, sizeof(label), "DSS[%u].derived", index);
        mismatch |= compare_bytes(label, &DeviceMeasure.dss[index].angle_x,
                                  expected_dss + item + 0x138u, 32u);
        (void)snprintf(label, sizeof(label), "DSS[%u].valid", index);
        mismatch |= compare_bytes(label, &DeviceMeasure.dss[index].valid_flag,
                                  expected_dss + item + 0x108u, 4u);
    }
    mismatch |= compare_bytes("GPS.position_velocity", DeviceMeasure.gps.position_gci,
                              expected_gps + 0x28u, 48u);
    mismatch |= compare_bytes("GPS.time", DeviceMeasure.gps.time_components,
                              expected_gps + 0x58u, 24u);
    return mismatch == 0 ? 0 : 1;
}

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
    if (file == NULL) { perror(path); return -1; }
    read_count = fread(out, 1u, bytes, file);
    return fclose(file) == 0 && read_count == bytes ? 0 : -1;
}

static int compare_blob(const char *label, const void *actual, const void *expected,
                        size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;

    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            fprintf(stderr, "%s mismatch at +0x%zx: actual=%02x expected=%02x\n", label,
                    index, (unsigned)a[index], (unsigned)e[index]);
            return 1;
        }
    }
    fprintf(stderr, "%s mismatched bytes: 0/%zu\n", label, bytes);
    return 0;
}

static void set_core_output(unsigned char core[0x148], int second)
{
    const DpQuat attitude = {1.0, 0.0, 0.0, 0.0};
    const DpVec3 body_rate = {0.01, -0.02, 0.03};
    const DpVec3 sun_body = {0.2, -0.4, 0.8};
    const DpVec3 sun_gci = {-1.0, 0.0, 0.0};
    const DpVec3 magnetic_body = {20.0, -30.0, 40.0};
    const DpVec3 position0 = {7000000.0, 1000.0, -500.0};
    const DpVec3 velocity0 = {1.5, -2.25, 3.75};
    const DpVec3 position1 = {7100000.25, -2000.5, 3000.75};
    const DpVec3 velocity1 = {-4.5, 5.25, -6.75};
    const double time0[6] = {2025.9, 1.1, 2.2, 3.3, 4.4, 5.5};
    const double time1[6] = {2031.9, 12.8, 31.7, 23.6, 59.5, 58.4};

    memset(core, 0, 0x148u);
    memcpy(core + 0x000u, &attitude, sizeof(attitude));
    memcpy(core + 0x020u, &body_rate, sizeof(body_rate));
    memcpy(core + 0x038u, &sun_body, sizeof(sun_body));
    memcpy(core + 0x050u, &sun_gci, sizeof(sun_gci));
    memcpy(core + 0x068u, &magnetic_body, sizeof(magnetic_body));
    memcpy(core + 0x098u, second != 0 ? &position1 : &position0, sizeof(position0));
    memcpy(core + 0x0b0u, second != 0 ? &velocity1 : &velocity0, sizeof(velocity0));
    memcpy(core + 0x0c8u, second != 0 ? time1 : time0, sizeof(time0));
}

int main(void)
{
    unsigned char core[0x148];
    unsigned char expected_step1[sizeof(DeviceMeasure.gps)];
    unsigned char expected_step2[sizeof(DeviceMeasure.gps)];
    int expected_flags[2];
    int mismatch = 0;

    if (read_blob("gold_sensor_init_gyro.bin", DeviceMeasure.gyro,
                  sizeof(DeviceMeasure.gyro)) != 0 ||
        read_blob("gold_sensor_init_magmeter.bin", DeviceMeasure.magmeter,
                  sizeof(DeviceMeasure.magmeter)) != 0 ||
        read_blob("gold_sensor_init_sts.bin", DeviceMeasure.sts,
                  sizeof(DeviceMeasure.sts)) != 0 ||
        read_blob("gold_sensor_init_dss.bin", DeviceMeasure.dss,
                  sizeof(DeviceMeasure.dss)) != 0 ||
        read_blob("gold_update_device_measure_gps_init_flag_p2_step1_gps.bin", expected_step1,
                  sizeof(expected_step1)) != 0 ||
        read_blob("gold_update_device_measure_gps_init_flag_p2_step2_gps.bin", expected_step2,
                  sizeof(expected_step2)) != 0 ||
        read_blob("gold_update_device_measure_gps_init_flag_p2_flags.bin", expected_flags,
                  sizeof(expected_flags)) != 0) {
        return 1;
    }
    memset(&DeviceMeasure.gps, 0, sizeof(DeviceMeasure.gps));
    DeviceMeasure.gps_init_flag = 1;
    dp_device_measure_globals_relocate();
    step_time = 0.1;

    set_core_output(core, 0);
    UpdateDeviceMeasure(core);
    mismatch |= compare_blob("UpdateDeviceMeasure GPS init-flag P2 step1 written tail",
                             (const unsigned char *)&DeviceMeasure.gps + 0x28u,
                             expected_step1 + 0x28u, sizeof(DeviceMeasure.gps) - 0x28u);
    mismatch |= compare_blob("UpdateDeviceMeasure GPS init-flag P2 step1 flag",
                             &DeviceMeasure.gps_init_flag, &expected_flags[0], sizeof(int));
    set_core_output(core, 1);
    UpdateDeviceMeasure(core);
    mismatch |= compare_blob("UpdateDeviceMeasure GPS init-flag P2 step2 written tail",
                             (const unsigned char *)&DeviceMeasure.gps + 0x28u,
                             expected_step2 + 0x28u, sizeof(DeviceMeasure.gps) - 0x28u);
    mismatch |= compare_blob("UpdateDeviceMeasure GPS init-flag P2 step2 flag",
                             &DeviceMeasure.gps_init_flag, &expected_flags[1], sizeof(int));
    if (mismatch == 0) {
        puts("UpdateDeviceMeasure GPS init-flag P2 original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_telemetry_layout.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) { perror(path); return -1; }
    if (fread(out, 1u, bytes, file) != bytes || fclose(file) != 0) return -1;
    return 0;
}

static void relocate_measure_pointers(DpDeviceMeasureRecovered *devices)
{
    unsigned index;
    for (index = 0u; index < 3u; ++index) {
        devices->sts[index].measure_quat.xyz.data =
            (double *)((uint8_t *)&devices->sts[index] + 0x158u);
    }
    for (index = 0u; index < 2u; ++index) {
        devices->gyro[index].measure.data =
            (double *)((uint8_t *)&devices->gyro[index] + 0x128u);
        devices->magmeter[index].measure.data =
            (double *)((uint8_t *)&devices->magmeter[index] + 0x120u);
    }
}

int main(void)
{
    DpDeviceMeasureRecovered devices;
    DpTelemetrySourceSnapshot source;
    DpMainTelemetryFrame actual;
    DpMainTelemetryFrame expected;
    size_t index;

    memset(&devices, 0, sizeof(devices));
    memset(&actual, 0, sizeof(actual));
    if (read_blob("gold_dyn_main_step1_sts.bin", devices.sts, sizeof(devices.sts)) != 0 ||
        read_blob("gold_dyn_main_step1_gyro.bin", devices.gyro, sizeof(devices.gyro)) != 0 ||
        read_blob("gold_dyn_main_step1_dss.bin", devices.dss, sizeof(devices.dss)) != 0 ||
        read_blob("gold_dyn_main_step1_magmeter.bin", devices.magmeter, sizeof(devices.magmeter)) != 0 ||
        read_blob("gold_dyn_main_step1_gps.bin", &devices.gps, sizeof(devices.gps)) != 0 ||
        read_blob("gold_dyn_main_step1_out.bin", &expected, sizeof(expected)) != 0) {
        return 1;
    }
    relocate_measure_pointers(&devices);
    if (dp_telemetry_source_from_devices(&source, &devices) != 0) {
        return 1;
    }
    dp_update_main_out_pack(&actual, &source);
    for (index = DP_TM_STS_VALID; index < DP_TM_RW_OMEGA; ++index) {
        if (actual.raw[index] != expected.raw[index]) {
            fprintf(stderr, "sensor offset +0x%zx got=%02x expected=%02x\n", index,
                    (unsigned)actual.raw[index], (unsigned)expected.raw[index]);
            return 1;
        }
    }
    for (index = DP_TM_GPS_VALUE; index < DP_TM_PROPAGATED_PREFIX; ++index) {
        if (actual.raw[index] != expected.raw[index]) {
            fprintf(stderr, "gps offset +0x%zx got=%02x expected=%02x\n", index,
                    (unsigned)actual.raw[index], (unsigned)expected.raw[index]);
            return 1;
        }
    }
    puts("UpdateMainOut real-device adapter original-ELF compare: PASS (sensor/GPS bytes; STS noise independently gold-tested)");
    return 0;
}

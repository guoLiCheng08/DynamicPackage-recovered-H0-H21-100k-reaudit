#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_devices.h"
#include "dynamic_sensors.h"
#include "dynamic_telemetry_layout.h"

#define GOLD_DIR "analysis/time_orbit/"
#define STS_BYTES (3u * 0x20u)

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    size_t count;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) { perror(path); return -1; }
    count = fread(out, 1u, bytes, file);
    return fclose(file) == 0 && count == bytes ? 0 : -1;
}

static int compare_sts(const char *label, const DpMainTelemetryFrame *actual,
                       const unsigned char expected[DP_MAIN_TELEMETRY_BYTES])
{
    const unsigned char *a = actual->raw + DP_TM_STS_QUAT_0;
    const unsigned char *e = expected + DP_TM_STS_QUAT_0;
    size_t index;

    for (index = 0u; index < STS_BYTES; ++index) {
        if (a[index] != e[index]) {
            fprintf(stderr, "%s mismatch at STS telemetry +0x%zx: actual=%02x expected=%02x\n",
                    label, index, (unsigned)a[index], (unsigned)e[index]);
            return 1;
        }
    }
    fprintf(stderr, "%s STS telemetry mismatched bytes: 0/%u\n", label, STS_BYTES);
    return 0;
}

static int run_case(const char *label, const unsigned int noise_flags[3],
                    const unsigned char expected[DP_MAIN_TELEMETRY_BYTES])
{
    DpMainTelemetryFrame output = {{0}};
    unsigned char core_output[0x148] = {0};
    unsigned index;

    dp_device_measure_globals_reset();
    dp_device_globals_reset();
    if (read_blob("gold_get_sts_quat_init_sts.bin", DeviceMeasure.sts,
                  sizeof(DeviceMeasure.sts)) != 0) {
        return 1;
    }
    dp_device_measure_globals_relocate();
    for (index = 0u; index < 3u; ++index) {
        DeviceMeasure.sts[index].gaussian_noise_flag = noise_flags[index];
    }
    srand(1u);
    UpdateMainOut(&output, core_output);
    return compare_sts(label, &output, expected);
}

int main(void)
{
    unsigned char expected_all_off[DP_MAIN_TELEMETRY_BYTES];
    unsigned char expected_ch1_on[DP_MAIN_TELEMETRY_BYTES];
    static const unsigned int all_off[3] = {0u, 0u, 0u};
    static const unsigned int channel1_on[3] = {0u, 1u, 0u};
    int mismatch;

    if (read_blob("gold_update_mainout_sts_all_off_p2_frame.bin", expected_all_off,
                  sizeof(expected_all_off)) != 0 ||
        read_blob("gold_update_mainout_sts_ch1_on_p2_frame.bin", expected_ch1_on,
                  sizeof(expected_ch1_on)) != 0) {
        return 1;
    }
    mismatch = run_case("UpdateMainOut STS P2 all-off", all_off, expected_all_off);
    mismatch |= run_case("UpdateMainOut STS P2 channel1-on", channel1_on, expected_ch1_on);
    if (mismatch == 0) {
        puts("UpdateMainOut STS switch P2 original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

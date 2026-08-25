#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_devices.h"
#include "dynamic_sensors.h"
#include "dynamic_telemetry_layout.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    size_t count;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    if (fclose(file) != 0 || count != bytes) {
        return -1;
    }
    return 0;
}

int main(void)
{
    DpMainTelemetryFrame output = {{0}};
    unsigned char core_output[0x148] = {0};
    double expected[4];
    static const char *const files[3] = {
        "gold_get_sts_quat_noise_0.bin",
        "gold_get_sts_quat_noise_1.bin",
        "gold_get_sts_quat_noise_2.bin"
    };
    unsigned index;

    dp_device_measure_globals_reset();
    dp_device_globals_reset();
    if (read_blob("gold_get_sts_quat_init_sts.bin", DeviceMeasure.sts,
                  sizeof(DeviceMeasure.sts)) != 0) {
        return 1;
    }
    dp_device_measure_globals_relocate();
    srand(1u);
    UpdateMainOut(&output, core_output);

    for (index = 0u; index < 3u; ++index) {
        const unsigned char *actual = output.raw + DP_TM_STS_QUAT_0 + index * 0x20u;
        if (read_blob(files[index], expected, sizeof(expected)) != 0 ||
            memcmp(actual, expected, sizeof(expected)) != 0) {
            unsigned byte;
            for (byte = 0u; byte < sizeof(expected); ++byte) {
                if (actual[byte] != ((const unsigned char *)expected)[byte]) {
                    printf("STS[%u] byte+0x%x actual=%02x expected=%02x\n", index, byte,
                           (unsigned)actual[byte],
                           (unsigned)((const unsigned char *)expected)[byte]);
                    break;
                }
            }
            return 1;
        }
    }
    puts("UpdateMainOut seeded STS original-ELF compare: PASS (bitwise)");
    return 0;
}

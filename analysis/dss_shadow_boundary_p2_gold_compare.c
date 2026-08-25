#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

#define GOLD_DIR "analysis/time_orbit/"
#define SAMPLE_COUNT 4u

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

int main(void)
{
    double positions[SAMPLE_COUNT][3];
    double measurement_data[2][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    double sun_data[3] = {1.0, 0.0, 0.0};
    DpDssRecovered sensors[2];
    DpVector sun = {3, 0, sun_data};
    int expected_flags[SAMPLE_COUNT][2];
    int expected_shadow[SAMPLE_COUNT];
    int actual_flags[SAMPLE_COUNT][2];
    int actual_shadow[SAMPLE_COUNT];
    unsigned sample;
    unsigned sensor;
    int mismatch = 0;

    if (read_blob("gold_dss_shadow_boundary_p2_positions.bin", positions,
                  sizeof(positions)) != 0 ||
        read_blob("gold_dss_shadow_boundary_p2_flags.bin", expected_flags,
                  sizeof(expected_flags)) != 0 ||
        read_blob("gold_dss_shadow_boundary_p2_shadow_results.bin", expected_shadow,
                  sizeof(expected_shadow)) != 0) {
        return 1;
    }
    memset(sensors, 0, sizeof(sensors));
    for (sensor = 0u; sensor < 2u; ++sensor) {
        sensors[sensor].valid_angle_threshold = 100.0;
        sensors[sensor].measure.count = 3;
        sensors[sensor].measure.data = measurement_data[sensor];
    }
    for (sample = 0u; sample < SAMPLE_COUNT; ++sample) {
        DpVector position = {3, 0, positions[sample]};

        actual_shadow[sample] = dp_is_earth_shadow(&sun, &position);
        dp_update_dss_valid_flag(sensors, &sun, &position);
        for (sensor = 0u; sensor < 2u; ++sensor) {
            actual_flags[sample][sensor] = sensors[sensor].valid_flag;
        }
    }
    mismatch |= compare_blob("DSS shadow-boundary P2 direct isEarthShadow", actual_shadow,
                             expected_shadow, sizeof(actual_shadow));
    mismatch |= compare_blob("DSS shadow-boundary P2 dual validity flags", actual_flags,
                             expected_flags, sizeof(actual_flags));
    if (mismatch == 0) {
        puts("DSS shadow-boundary P2 original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

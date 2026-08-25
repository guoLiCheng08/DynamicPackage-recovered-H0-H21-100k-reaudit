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
    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) { perror(path); return -1; }
    if (fread(out, 1u, bytes, file) != bytes || fclose(file) != 0) return -1;
    return 0;
}

static void relocate_sts(DpStsRecovered sensors[3])
{
    unsigned index;
    for (index = 0u; index < 3u; ++index) {
        sensors[index].error_quat.xyz.data =
            (double *)((uint8_t *)&sensors[index] + 0x118u);
        sensors[index].measure_quat.xyz.data =
            (double *)((uint8_t *)&sensors[index] + 0x158u);
    }
}

static int compare_one(const char *file_name, const double actual[4])
{
    double expected[4];
    unsigned index;
    if (read_blob(file_name, expected, sizeof(expected)) != 0) return 1;
    for (index = 0u; index < 4u; ++index) {
        uint64_t a;
        uint64_t e;
        memcpy(&a, &actual[index], sizeof(a));
        memcpy(&e, &expected[index], sizeof(e));
        if (a != e) {
            fprintf(stderr, "%s[%u] got=%016llx expected=%016llx\n", file_name, index,
                    (unsigned long long)a, (unsigned long long)e);
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    DpStsRecovered sensors[3];
    double output[4];

    if (read_blob("gold_get_sts_quat_init_sts.bin", sensors, sizeof(sensors)) != 0) return 1;
    relocate_sts(sensors);
    sensors[0].gaussian_noise_flag = 0u;
    if (dp_get_star_tracker_quat(output, &sensors[0], NULL, NULL) != 0 ||
        compare_one("gold_get_sts_quat_no_noise.bin", output) != 0) return 1;

    if (read_blob("gold_get_sts_quat_init_sts.bin", sensors, sizeof(sensors)) != 0) return 1;
    relocate_sts(sensors);
    sensors[0].gaussian_noise_flag = 1u;
    srand(1u);
    if (dp_get_star_tracker_quat(output, &sensors[0], NULL, NULL) != 0 ||
        compare_one("gold_get_sts_quat_noise_0.bin", output) != 0 ||
        dp_get_star_tracker_quat(output, &sensors[1], NULL, NULL) != 0 ||
        compare_one("gold_get_sts_quat_noise_1.bin", output) != 0 ||
        dp_get_star_tracker_quat(output, &sensors[2], NULL, NULL) != 0 ||
        compare_one("gold_get_sts_quat_noise_2.bin", output) != 0) return 1;
    puts("GetStarTrackerQuat original-ELF gold compare: PASS (bitwise)");
    return 0;
}

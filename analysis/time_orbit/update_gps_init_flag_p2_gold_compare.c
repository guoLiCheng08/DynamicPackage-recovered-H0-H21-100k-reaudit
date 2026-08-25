#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_sensors.h"

#define GOLD_DIR "analysis/time_orbit/"
#define CALL_COUNT 4u
#define OBJECT_TAIL_OFFSET 0x28u
#define OBJECT_TAIL_BYTES (sizeof(DpGpsKalmanRecovered) - OBJECT_TAIL_OFFSET)
#define SNAPSHOT_BYTES (CALL_COUNT * sizeof(DpGpsKalmanRecovered))
#define TAIL_SNAPSHOT_BYTES (CALL_COUNT * OBJECT_TAIL_BYTES)
#define FLAG_BYTES (CALL_COUNT * sizeof(int))

static int read_exact(const char *name, void *out, size_t bytes)
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

static int compare_blob(const char *label, const unsigned char *actual,
                        const unsigned char *expected, size_t bytes)
{
    size_t index;
    unsigned mismatches = 0u;

    for (index = 0u; index < bytes; ++index) {
        if (actual[index] != expected[index]) {
            if (mismatches < 8u) {
                printf("%s byte+0x%zx actual=%02x expected=%02x\n", label, index,
                       (unsigned)actual[index], (unsigned)expected[index]);
            }
            ++mismatches;
        }
    }
    printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

int main(void)
{
    static const int initial_flags[CALL_COUNT] = {1, 0, 2, -1};
    unsigned char expected_snapshots[SNAPSHOT_BYTES];
    unsigned char expected_tails[TAIL_SNAPSHOT_BYTES];
    unsigned char actual_tails[TAIL_SNAPSHOT_BYTES];
    int expected_flags[CALL_COUNT];
    int actual_flags[CALL_COUNT];
    DpGpsKalmanRecovered gps;
    unsigned index;
    int mismatch = 0;

    if (read_exact("gold_update_gps_init_flag_p2_snapshots.bin", expected_snapshots,
                   sizeof(expected_snapshots)) != 0 ||
        read_exact("gold_update_gps_init_flag_p2_flags.bin", expected_flags,
                   sizeof(expected_flags)) != 0) {
        return 1;
    }
    memset(&gps, 0, sizeof(gps));
    for (index = 0u; index < CALL_COUNT; ++index) {
        const double time_values[6] = {
            2025.9 + (double)index, -1.9 - (double)index, 3.1 + (double)index,
            4.9 - (double)index, 5.0 + (double)index, 6.99 - (double)index};
        const DpVec3 position = {7000000.25 + (double)index,
                                 -123.5 - (double)index,
                                 42.125 + (double)index};
        const DpVec3 velocity = {1.5 + (double)index,
                                 -2.25 - (double)index,
                                 3.75 + (double)index};
        int init_flag = initial_flags[index];

        dp_update_gps(&gps, time_values, &position, &velocity, &init_flag);
        memcpy(actual_tails + index * OBJECT_TAIL_BYTES,
               (const unsigned char *)&gps + OBJECT_TAIL_OFFSET, OBJECT_TAIL_BYTES);
        memcpy(expected_tails + index * OBJECT_TAIL_BYTES,
               expected_snapshots + index * sizeof(gps) + OBJECT_TAIL_OFFSET,
               OBJECT_TAIL_BYTES);
        actual_flags[index] = init_flag;
    }
    mismatch |= compare_blob("UpdateGPS P2 written object tails", actual_tails,
                             expected_tails, sizeof(actual_tails));
    mismatch |= compare_blob("UpdateGPS P2 init flags",
                             (const unsigned char *)actual_flags,
                             (const unsigned char *)expected_flags,
                             sizeof(actual_flags));
    if (mismatch == 0) {
        puts("UpdateGPS init-flag P2 original-ELF compare: PASS (bitwise)");
    }
    return mismatch == 0 ? 0 : 1;
}

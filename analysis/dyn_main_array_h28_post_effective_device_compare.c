#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_devices.h"
#include "dynamic_satellite_globals.h"

#define GOLD_DIR "analysis/time_orbit/"

typedef struct {
    size_t offset;
    size_t bytes;
} DpRange;

static int read_gold(const char *name, void *out, size_t bytes)
{
    char path[512];
    FILE *file;
    size_t count;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    return fclose(file) != 0 || count != bytes ? -1 : 0;
}

static int compare_ranges(const char *label, const void *actual, const void *expected,
                          const DpRange *ranges, size_t count)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t r;
    unsigned mismatches = 0u;

    for (r = 0u; r < count; ++r) {
        size_t i;
        for (i = 0u; i < ranges[r].bytes; ++i) {
            const size_t offset = ranges[r].offset + i;
            if (a[offset] != e[offset]) {
                if (mismatches < 8u) {
                    (void)printf("%s byte+0x%zx actual=%02x expected=%02x\n", label, offset,
                                 (unsigned)a[offset], (unsigned)e[offset]);
                }
                ++mismatches;
            }
        }
    }
    (void)printf("%s effective mismatched bytes: %u\n", label, mismatches);
    return mismatches == 0u ? 0 : -1;
}

int main(void)
{
    const double initial[30] = {
        0.01, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        2024.0, 10.0, 28.0, 4.0, 16.0, 16.0,
        6910593.0, 0.00058, 1.7018327148814874, 0.24732846154513649,
        3.0651957525056472, 6.2273473025797976,
        120.0, -0.63, 1.3, -0.63, 361.0, -0.5, 1.3, -0.5, 469.0, 600.0
    };
    const DpRange rwheel_ranges[] = {
        {0x04u, 0x4cu}, {0x60u, 0x18u},
        {0x7cu, 0x4cu}, {0xd8u, 0x18u},
        {0xf4u, 0x4cu}, {0x150u, 0x18u},
        {0x16cu, 0x4cu}, {0x1c8u, 0x18u}
    };
    const DpRange mtq_ranges[] = {
        {0x00u, 0x10u}, {0x10u, 0x04u}, {0x20u, 0x18u},
        {0x38u, 0x10u}, {0x48u, 0x04u}, {0x58u, 0x18u},
        {0x70u, 0x10u}, {0x80u, 0x04u}, {0x90u, 0x18u},
        {0xa8u, 0x10u}, {0xb8u, 0x04u}, {0xc8u, 0x18u},
        {0xe0u, 0x10u}, {0xf0u, 0x04u}, {0x100u, 0x18u},
        {0x118u, 0x10u}, {0x128u, 0x04u}, {0x138u, 0x18u}
    };
    const DpRange thruster_ranges[] = {
        {0x00u, 0x10u}, {0x10u, 0x04u}, {0x20u, 0x18u},
        {0x38u, 0x04u}, {0x48u, 0x18u}, {0x60u, 0x04u}, {0x70u, 0x18u},
        {0x88u, 0x04u}, {0x98u, 0x18u}
    };
    double command[16] = {0.0};
    double output_words[0x1e8u / sizeof(double)];
    double state_words[0x200u / sizeof(double)];
    unsigned char expected_rwheel[sizeof(RWheel)];
    unsigned char expected_mtq[sizeof(MTQ)];
    unsigned char expected_thruster[sizeof(Thruster)];
    unsigned char expected_sada[sizeof(SADA)];
    int mismatch;

    if (read_gold("gold_dyn_main_array_h28_post_rwheel.bin", expected_rwheel,
                  sizeof(expected_rwheel)) != 0 ||
        read_gold("gold_dyn_main_array_h28_post_mtq.bin", expected_mtq,
                  sizeof(expected_mtq)) != 0 ||
        read_gold("gold_dyn_main_array_h28_post_thruster.bin", expected_thruster,
                  sizeof(expected_thruster)) != 0 ||
        read_gold("gold_dyn_main_array_h28_post_sada.bin", expected_sada,
                  sizeof(expected_sada)) != 0) {
        return 1;
    }
    command[0] = 0.005;
    command[2] = -0.003;
    command[4] = 0.008;
    command[7] = -0.006;
    command[10] = 1.0;
    command[11] = 0.01;
    command[12] = -0.005;
    command[14] = 1.0;
    command[15] = 1.0;
    dyn_init_array(initial);
    memset(output_words, 0xa5, sizeof(output_words));
    memset(state_words, 0x5a, sizeof(state_words));
    dyn_main_array(output_words, state_words, command);
    mismatch = compare_ranges("H28 post RWheel", RWheel, expected_rwheel,
                              rwheel_ranges, sizeof(rwheel_ranges) / sizeof(rwheel_ranges[0]));
    mismatch |= compare_ranges("H28 post MTQ", MTQ, expected_mtq,
                               mtq_ranges, sizeof(mtq_ranges) / sizeof(mtq_ranges[0]));
    mismatch |= compare_ranges("H28 post Thruster", &Thruster, expected_thruster,
                               thruster_ranges, sizeof(thruster_ranges) / sizeof(thruster_ranges[0]));
    mismatch |= compare_ranges("H28 post SADA", &SADA, expected_sada,
                               (const DpRange[]){{0u, sizeof(SADA)}}, 1u);
    return mismatch == 0 ? 0 : 1;
}

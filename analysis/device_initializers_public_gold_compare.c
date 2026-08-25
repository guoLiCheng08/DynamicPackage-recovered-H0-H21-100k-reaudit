#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

static int mismatch_bytes(const void *actual, const void *expected, size_t bytes,
                          const char *label)
{
    if (memcmp(actual, expected, bytes) != 0) {
        (void)printf("%s mismatched bytes\n", label);
        return -1;
    }
    return 0;
}

static int pointer_offset_matches(const void *base, const void *pointer,
                                  size_t expected, const char *label)
{
    const uintptr_t actual_offset = (uintptr_t)pointer - (uintptr_t)base;

    if (actual_offset != expected) {
        (void)printf("%s offset=%#llx expected=%#llx\n", label,
                     (unsigned long long)actual_offset,
                     (unsigned long long)expected);
        return -1;
    }
    return 0;
}

static void prepare_gyro(unsigned index, const double source[9])
{
    unsigned char *base = (unsigned char *)&Gyro[index];

    Gyro[index].installation_matrix = (DpMatrix){3, 3, 3, 0, NULL};
    Gyro[index].projection_matrix = (DpMatrix){3, 3, 3, 0, NULL};
    Gyro[index].measure = (DpVector){3, 0, NULL};
    memcpy(base + 0x80u, source, 9u * sizeof(source[0]));
}

static void prepare_magmeter(unsigned index, const double source[9])
{
    unsigned char *base = (unsigned char *)&MagMeter[index];

    MagMeter[index].installation_matrix = (DpMatrix){3, 3, 3, 0, NULL};
    MagMeter[index].projection_matrix = (DpMatrix){3, 3, 3, 0, NULL};
    MagMeter[index].measure = (DpVector){3, 0, NULL};
    memcpy(base + 0x78u, source, 9u * sizeof(source[0]));
}

static void prepare_dss(unsigned index, const double source[9])
{
    unsigned char *base = (unsigned char *)&DSS[index];

    DSS[index].installation_matrix = (DpMatrix){3, 3, 3, 0, NULL};
    DSS[index].projection_matrix = (DpMatrix){3, 3, 3, 0, NULL};
    DSS[index].measure = (DpVector){3, 0, NULL};
    memcpy(base + 0x70u, source, 9u * sizeof(source[0]));
}

int main(void)
{
    static const double source_0[9] = {
        1.25, -2.5, 3.75,
        -4.125, 5.5, -6.625,
        7.875, -8.25, 9.5
    };
    static const double source_1[9] = {
        -1.5, 2.25, -3.0,
        4.75, -5.25, 6.5,
        -7.75, 8.125, 9.875
    };
    static const uint64_t expected_0_bits[9] = {
        UINT64_C(0x3ff4000000000000), UINT64_C(0xc010800000000000),
        UINT64_C(0x401f800000000000), UINT64_C(0xc004000000000000),
        UINT64_C(0x4016000000000000), UINT64_C(0xc020800000000000),
        UINT64_C(0x400e000000000000), UINT64_C(0xc01a800000000000),
        UINT64_C(0x4023000000000000)
    };
    static const uint64_t expected_1_bits[9] = {
        UINT64_C(0xbff8000000000000), UINT64_C(0x4013000000000000),
        UINT64_C(0xc01f000000000000), UINT64_C(0x4002000000000000),
        UINT64_C(0xc015000000000000), UINT64_C(0x4020400000000000),
        UINT64_C(0xc008000000000000), UINT64_C(0x401a000000000000),
        UINT64_C(0x4023c00000000000)
    };
    unsigned index;

    memset(Gyro, 0, sizeof(Gyro));
    memset(MagMeter, 0, sizeof(MagMeter));
    memset(DSS, 0, sizeof(DSS));
    prepare_gyro(0u, source_0);
    prepare_gyro(1u, source_1);
    prepare_magmeter(0u, source_0);
    prepare_magmeter(1u, source_1);
    prepare_dss(0u, source_0);
    prepare_dss(1u, source_1);
    Gyro_Init();
    MagMeter_Init();
    DSS_Init();

    for (index = 0u; index < 2u; ++index) {
        const uint64_t *expected = index == 0u ? expected_0_bits : expected_1_bits;
        const char *suffix = index == 0u ? "0" : "1";
        unsigned char *gyro_base = (unsigned char *)&Gyro[index];
        unsigned char *mag_base = (unsigned char *)&MagMeter[index];
        unsigned char *dss_base = (unsigned char *)&DSS[index];

        if (pointer_offset_matches(gyro_base, Gyro[index].installation_matrix.data,
                                   0x80u, "gyro.installation") != 0 ||
            pointer_offset_matches(gyro_base, Gyro[index].projection_matrix.data,
                                   0xc8u, "gyro.projection") != 0 ||
            pointer_offset_matches(gyro_base, Gyro[index].measure.data,
                                   0x128u, "gyro.measure") != 0 ||
            pointer_offset_matches(mag_base, MagMeter[index].installation_matrix.data,
                                   0x78u, "mag.installation") != 0 ||
            pointer_offset_matches(mag_base, MagMeter[index].projection_matrix.data,
                                   0xc0u, "mag.projection") != 0 ||
            pointer_offset_matches(mag_base, MagMeter[index].measure.data,
                                   0x120u, "mag.measure") != 0 ||
            pointer_offset_matches(dss_base, DSS[index].installation_matrix.data,
                                   0x70u, "dss.installation") != 0 ||
            pointer_offset_matches(dss_base, DSS[index].projection_matrix.data,
                                   0xb8u, "dss.projection") != 0 ||
            pointer_offset_matches(dss_base, DSS[index].measure.data,
                                   0x120u, "dss.measure") != 0 ||
            mismatch_bytes(gyro_base + 0xc8u, expected, sizeof(expected_0_bits),
                           suffix) != 0 ||
            mismatch_bytes(mag_base + 0xc0u, expected, sizeof(expected_0_bits),
                           suffix) != 0 ||
            mismatch_bytes(dss_base + 0xb8u, expected, sizeof(expected_0_bits),
                           suffix) != 0) {
            return 1;
        }
    }
    (void)puts("PASS: Gyro_Init/MagMeter_Init/DSS_Init controlled original-ELF gold compare");
    return 0;
}

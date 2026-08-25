#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_sensors.h"

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

static int compare_quat(const DpQuatAbi *actual, const uint64_t expected[4],
                        const char *label)
{
    uint64_t bits[4];

    memcpy(&bits[0], &actual->w, sizeof(bits[0]));
    memcpy(&bits[1], &actual->xyz.data[0], sizeof(bits[1]));
    memcpy(&bits[2], &actual->xyz.data[1], sizeof(bits[2]));
    memcpy(&bits[3], &actual->xyz.data[2], sizeof(bits[3]));
    if (memcmp(bits, expected, sizeof(bits)) != 0) {
        (void)printf("%s bits mismatch\n", label);
        return -1;
    }
    return 0;
}

int main(void)
{
    static const double matrices[3][9] = {
        {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0},
        {0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}
    };
    static const uint64_t expected_installation[3][4] = {
        {UINT64_C(0x3ff0000000000000), 0u, 0u, 0u},
        {0u, UINT64_C(0x3ff0000000000000), 0u, 0u},
        {UINT64_C(0x3fe6a09e667f3bcd), 0u, 0u, UINT64_C(0xbfe6a09e667f3bcc)}
    };
    static const uint64_t expected_error[3][4] = {
        {UINT64_C(0x3ff0000000000000), UINT64_C(0x8000000000000000),
         UINT64_C(0x8000000000000000), UINT64_C(0x8000000000000000)},
        {0u, UINT64_C(0xbff0000000000000), UINT64_C(0x8000000000000000),
         UINT64_C(0x8000000000000000)},
        {UINT64_C(0x3fe6a09e667f3bcd), UINT64_C(0x8000000000000000),
         UINT64_C(0x8000000000000000), UINT64_C(0x3fe6a09e667f3bcc)}
    };
    unsigned index;

    memset(STS, 0, sizeof(STS));
    for (index = 0u; index < 3u; ++index) {
        unsigned char *base = (unsigned char *)&STS[index];
        STS[index].installation_matrix = (DpMatrix){3, 3, 3, 0, NULL};
        memcpy(base + 0x78u, matrices[index], sizeof(matrices[index]));
        ((double *)(void *)(base + 0x158u))[0] = 123.25 + (double)index;
        ((double *)(void *)(base + 0x158u))[1] = -456.5 - (double)index;
        ((double *)(void *)(base + 0x158u))[2] = 789.75 + (double)index;
    }
    STS_Init();

    for (index = 0u; index < 3u; ++index) {
        unsigned char *base = (unsigned char *)&STS[index];
        const double expected_measure[3] = {
            123.25 + (double)index, -456.5 - (double)index, 789.75 + (double)index
        };

        if (pointer_offset_matches(base, STS[index].installation_matrix.data,
                                   0x78u, "matrix") != 0 ||
            pointer_offset_matches(base, STS[index].installation_quat.xyz.data,
                                   0xe0u, "install-quat") != 0 ||
            pointer_offset_matches(base, STS[index].error_quat.xyz.data,
                                   0x118u, "error-quat") != 0 ||
            pointer_offset_matches(base, STS[index].measure_quat.xyz.data,
                                   0x158u, "measure-quat") != 0 ||
            STS[index].installation_quat.xyz.count != 3 ||
            STS[index].error_quat.xyz.count != 3 || STS[index].measure_quat.xyz.count != 3 ||
            compare_quat(&STS[index].installation_quat, expected_installation[index],
                         "installation") != 0 ||
            compare_quat(&STS[index].error_quat, expected_error[index], "error") != 0 ||
            memcmp(STS[index].measure_quat.xyz.data, expected_measure,
                   sizeof(expected_measure)) != 0 || STS[index].measure_quat.w != 1.0) {
            return 1;
        }
    }
    (void)puts("PASS: STS_Init controlled original-ELF gold compare");
    return 0;
}

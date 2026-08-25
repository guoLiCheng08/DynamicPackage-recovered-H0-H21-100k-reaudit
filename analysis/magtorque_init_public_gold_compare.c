#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"

int main(void)
{
    double axis[6][3] = {
        {1.25, -2.5, 3.75},
        {4.125, -5.5, 6.625},
        {-7.875, 8.25, -9.5},
        {10.75, -11.125, 12.5},
        {-13.75, 14.125, -15.5},
        {16.75, -17.25, 18.5}
    };
    double actual[18] = {0.0};
    static const uint64_t expected_bits[18] = {
        UINT64_C(0x3ff4000000000000), UINT64_C(0x4010800000000000),
        UINT64_C(0xc01f800000000000), UINT64_C(0x4025800000000000),
        UINT64_C(0xc02b800000000000), UINT64_C(0x4030c00000000000),
        UINT64_C(0xc004000000000000), UINT64_C(0xc016000000000000),
        UINT64_C(0x4020800000000000), UINT64_C(0xc026400000000000),
        UINT64_C(0x402c400000000000), UINT64_C(0xc031400000000000),
        UINT64_C(0x400e000000000000), UINT64_C(0x401a800000000000),
        UINT64_C(0xc023000000000000), UINT64_C(0x4029000000000000),
        UINT64_C(0xc02f000000000000), UINT64_C(0x4032800000000000)
    };
    unsigned index;

    memset(MTQ, 0, sizeof(MTQ));
    memset(&MTQ_Group, 0, sizeof(MTQ_Group));
    for (index = 0u; index < 6u; ++index) {
        MTQ[index].installation_axis = (DpVector){3, 0, axis[index]};
    }
    MTQ_Group.mapping_3x6 = (DpMatrix){3, 6, 6, 0, actual};
    MagTorque_Init();

    if (memcmp(actual, expected_bits, sizeof(actual)) != 0) {
        (void)puts("MagTorque_Init matrix bits mismatch");
        return 1;
    }
    for (index = 0u; index < 6u; ++index) {
        if (MTQ[index].installation_axis.count != 3 ||
            MTQ[index].installation_axis.data != axis[index]) {
            (void)puts("MagTorque_Init changed installation-axis descriptor");
            return 1;
        }
    }
    if (MTQ_Group.mapping_3x6.rows != 3 || MTQ_Group.mapping_3x6.cols != 6 ||
        MTQ_Group.mapping_3x6.row_stride != 6 || MTQ_Group.mapping_3x6.data != actual) {
        (void)puts("MagTorque_Init changed mapping descriptor");
        return 1;
    }
    (void)puts("PASS: MagTorque_Init controlled original-ELF gold compare");
    return 0;
}

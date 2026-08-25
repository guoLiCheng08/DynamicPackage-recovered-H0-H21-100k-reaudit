#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"

int main(void)
{
    double axis[4][3] = {
        {1.25, -2.5, 3.75},
        {4.125, -5.5, 6.625},
        {-7.875, 8.25, -9.5},
        {10.75, -11.125, 12.5}
    };
    double actual[12] = {0.0};
    static const uint64_t expected_bits[12] = {
        UINT64_C(0x3ff4000000000000), UINT64_C(0x4010800000000000),
        UINT64_C(0xc01f800000000000), UINT64_C(0x4025800000000000),
        UINT64_C(0xc004000000000000), UINT64_C(0xc016000000000000),
        UINT64_C(0x4020800000000000), UINT64_C(0xc026400000000000),
        UINT64_C(0x400e000000000000), UINT64_C(0x401a800000000000),
        UINT64_C(0xc023000000000000), UINT64_C(0x4029000000000000)
    };
    unsigned index;

    memset(RWheel, 0, sizeof(RWheel));
    memset(&WheelGroup, 0, sizeof(WheelGroup));
    for (index = 0u; index < 4u; ++index) {
        RWheel[index].installation_axis = (DpVector){3, 0, axis[index]};
    }
    WheelGroup.mapping_3x4 = (DpMatrix){3, 4, 4, 0, actual};
    Wheel_Init();

    if (memcmp(actual, expected_bits, sizeof(actual)) != 0) {
        (void)puts("Wheel_Init matrix bits mismatch");
        return 1;
    }
    for (index = 0u; index < 4u; ++index) {
        if (RWheel[index].installation_axis.count != 3 ||
            RWheel[index].installation_axis.data != axis[index]) {
            (void)puts("Wheel_Init changed installation-axis descriptor");
            return 1;
        }
    }
    if (WheelGroup.mapping_3x4.rows != 3 || WheelGroup.mapping_3x4.cols != 4 ||
        WheelGroup.mapping_3x4.row_stride != 4 || WheelGroup.mapping_3x4.data != actual) {
        (void)puts("Wheel_Init changed mapping descriptor");
        return 1;
    }
    (void)puts("PASS: Wheel_Init controlled original-ELF gold compare");
    return 0;
}

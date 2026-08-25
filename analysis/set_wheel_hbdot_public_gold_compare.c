#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"

int main(void)
{
    double input_data[3] = {1.25, -2.5, 3.75};
    double torque_data[3] = {0.0, 0.0, 0.0};
    static const uint64_t expected_bits[3] = {
        UINT64_C(0x3ff4000000000000),
        UINT64_C(0xc004000000000000),
        UINT64_C(0x400e000000000000)
    };
    DpVector input = {3, 0, input_data};

    memset(&WheelGroup, 0, sizeof(WheelGroup));
    WheelGroup.torque = (DpVector){3, 0, torque_data};
    SetWheel_HBdot(&input);

    if (memcmp(torque_data, expected_bits, sizeof(torque_data)) != 0 ||
        WheelGroup.torque.count != 3 || WheelGroup.torque.reserved_04 != 0 ||
        WheelGroup.torque.data != torque_data) {
        (void)puts("SetWheel_HBdot controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: SetWheel_HBdot controlled original-ELF gold compare");
    return 0;
}

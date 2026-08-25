#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"

int main(void)
{
    double source[3] = {1.25, -2.5, 3.75};
    double output[3] = {0.0, 0.0, 0.0};
    static const uint64_t expected_bits[3] = {
        UINT64_C(0x3ff4000000000000),
        UINT64_C(0xc004000000000000),
        UINT64_C(0x400e000000000000)
    };
    DpVector destination = {3, 0, output};

    memset(&WheelGroup, 0, sizeof(WheelGroup));
    WheelGroup.angular_momentum = (DpVector){3, 0, source};
    GetWheelMoment(&destination);

    if (memcmp(output, expected_bits, sizeof(output)) != 0 ||
        destination.count != 3 || destination.reserved_04 != 0 ||
        destination.data != output) {
        (void)puts("GetWheelMoment controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: GetWheelMoment controlled original-ELF gold compare");
    return 0;
}

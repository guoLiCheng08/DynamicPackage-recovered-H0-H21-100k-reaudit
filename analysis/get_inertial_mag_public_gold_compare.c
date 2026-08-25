#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

int main(void)
{
    double magnetic_data[3] = {1.25, -2.5, 3.75};
    double output_data[3] = {0.0, 0.0, 0.0};
    static const uint64_t expected_bits[3] = {
        UINT64_C(0x3ff4000000000000),
        UINT64_C(0xc004000000000000),
        UINT64_C(0x400e000000000000)
    };
    DpVector output = {3, 0, output_data};

    B_I_static = (DpVector){3, 0, magnetic_data};
    GetInertialMag(&output);
    if (memcmp(output_data, expected_bits, sizeof(output_data)) != 0 ||
        output.count != 3 || output.reserved_04 != 0 || output.data != output_data) {
        (void)puts("GetInertialMag controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: GetInertialMag controlled original-ELF gold compare");
    return 0;
}

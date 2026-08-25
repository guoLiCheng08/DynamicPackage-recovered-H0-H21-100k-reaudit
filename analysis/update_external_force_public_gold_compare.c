#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"

int main(void)
{
    double input_data[3] = {1.25, -2.5, 3.75};
    double force_data[3] = {0.0, 0.0, 0.0};
    static const uint64_t expected_bits[3] = {
        UINT64_C(0x3ff4000000000000),
        UINT64_C(0xc004000000000000),
        UINT64_C(0x400e000000000000)
    };
    DpVector input = {3, 0, input_data};

    F_I_external = (DpVector){3, 0, force_data};
    UpdateExternalForce(&input);
    if (memcmp(force_data, expected_bits, sizeof(force_data)) != 0 ||
        F_I_external.count != 3 || F_I_external.reserved_04 != 0 ||
        F_I_external.data != force_data) {
        (void)puts("UpdateExternalForce controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: UpdateExternalForce controlled original-ELF gold compare");
    return 0;
}

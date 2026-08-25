#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_environment.h"
#include "dynamic_satellite_globals.h"

int main(void)
{
    DpCalendarTime calendar = {2024.0, 2.0, 29.0, 23.0, 59.0, 59.5};
    double magnetic_data[3] = {0.0, 0.0, 0.0};
    static const uint64_t expected_bits[3] = {
        UINT64_C(0xbeefbf3df443db5d),
        UINT64_C(0x3ed6db0fcf0f6860),
        UINT64_C(0x3eed4bea300be60e)
    };

    y[7] = 7000000.0;
    y[8] = -1200000.0;
    y[9] = 2300000.0;
    y[10] = 1200.0;
    y[11] = 7300.0;
    y[12] = -1800.0;
    B_I_static = (DpVector){3, 0, magnetic_data};
    MagUpdate(&calendar);
    if (memcmp(magnetic_data, expected_bits, sizeof(magnetic_data)) != 0 ||
        B_I_static.count != 3 || B_I_static.reserved_04 != 0 ||
        B_I_static.data != magnetic_data) {
        (void)puts("MagUpdate controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: MagUpdate controlled original-ELF gold compare");
    return 0;
}

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_environment.h"

int main(void)
{
    double output_data[3] = {0.0, 0.0, 0.0};
    DpVector output = {3, 0, output_data};
    const DpCalendarTime calendar = {2024.0, 2.0, 29.0, 23.0, 59.0, 59.5};
    static const uint64_t expected_bits[3] = {
        UINT64_C(0x424043c08bd4dda4),
        UINT64_C(0xc22527a00b79afd9),
        UINT64_C(0xc21257ebf70135e6)
    };

    GetSunVector(&output, &calendar);
    if (memcmp(output_data, expected_bits, sizeof(output_data)) != 0 ||
        output.count != 3 || output.reserved_04 != 0 || output.data != output_data) {
        (void)puts("GetSunVector controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: GetSunVector controlled original-ELF gold compare");
    return 0;
}

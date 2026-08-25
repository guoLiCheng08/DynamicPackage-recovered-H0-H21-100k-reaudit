#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

int main(void)
{
    double quat_xyz[3] = {-2.5, 4.125, 3.75};
    double psi_data[12] = {0.0};
    DpQuatAbi quat = {1.25, {3, 0, quat_xyz}};
    DpMatrix psi = {4, 3, 3, 0, psi_data};
    static const uint64_t expected_bits[12] = {
        UINT64_C(0x3ff4000000000000), UINT64_C(0x400e000000000000),
        UINT64_C(0xc010800000000000), UINT64_C(0xc00e000000000000),
        UINT64_C(0x3ff4000000000000), UINT64_C(0xc004000000000000),
        UINT64_C(0x4010800000000000), UINT64_C(0x4004000000000000),
        UINT64_C(0x3ff4000000000000), UINT64_C(0x4004000000000000),
        UINT64_C(0xc010800000000000), UINT64_C(0xc00e000000000000)
    };

    quat_psi(&psi, &quat);
    if (memcmp(psi_data, expected_bits, sizeof(psi_data)) != 0 ||
        psi.rows != 4 || psi.cols != 3 || psi.row_stride != 3 ||
        psi.reserved_0c != 0 || psi.data != psi_data) {
        (void)puts("quat_psi controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: quat_psi controlled original-ELF gold compare");
    return 0;
}

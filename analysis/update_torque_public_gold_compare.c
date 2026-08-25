#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

static void configure_vector(size_t descriptor_offset)
{
    DpVector descriptor;

    memcpy(&descriptor, &SatTorque.raw[descriptor_offset], sizeof(descriptor));
    descriptor.count = 3;
    descriptor.reserved_04 = 0;
    memcpy(&SatTorque.raw[descriptor_offset], &descriptor, sizeof(descriptor));
}

static void store_three(size_t data_offset, const double value[3])
{
    memcpy(&SatTorque.raw[data_offset], value, 3u * sizeof(value[0]));
}

int main(void)
{
    static const double term_0[3] = {1.25, -2.5, 3.75};
    static const double term_1[3] = {-4.0, 5.5, 6.25};
    static const double term_2[3] = {7.0, -8.5, 9.125};
    static const uint64_t expected_bits[3] = {
        UINT64_C(0x4011000000000000),
        UINT64_C(0xc016000000000000),
        UINT64_C(0x4033200000000000)
    };
    double torque_data[3] = {0.0, 0.0, 0.0};
    double observed_total[3];
    DpVector total;

    memset(&SatTorque, 0, sizeof(SatTorque));
    TorqueInit();
    configure_vector(0x00u);
    configure_vector(0x10u);
    configure_vector(0x20u);
    configure_vector(0x70u);
    store_three(0x80u, term_0);
    store_three(0x98u, term_1);
    store_three(0xb0u, term_2);
    L_c_B = (DpVector){3, 0, torque_data};

    UpdateTorque();
    memcpy(observed_total, &SatTorque.raw[0x128], sizeof(observed_total));
    memcpy(&total, &SatTorque.raw[0x70], sizeof(total));
    if (memcmp(observed_total, expected_bits, sizeof(observed_total)) != 0 ||
        memcmp(torque_data, expected_bits, sizeof(torque_data)) != 0 ||
        total.count != 3 || total.reserved_04 != 0 ||
        total.data != (double *)(void *)&SatTorque.raw[0x128]) {
        (void)puts("UpdateTorque controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: UpdateTorque controlled original-ELF gold compare");
    return 0;
}

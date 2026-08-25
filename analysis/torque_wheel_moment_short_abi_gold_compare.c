#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

int main(void)
{
    double torque_input_data[3] = {1.25, -2.5, 3.75};
    double torque_global_data[3] = {0.0, 0.0, 0.0};
    double torque_output_data[3] = {0.0, 0.0, 0.0};
    double wheel_input_data[3] = {-4.125, 5.5, -6.625};
    double wheel_global_data[3] = {0.0, 0.0, 0.0};
    double wheel_output_data[3] = {0.0, 0.0, 0.0};
    static const uint64_t torque_bits[3] = {
        UINT64_C(0x3ff4000000000000),
        UINT64_C(0xc004000000000000),
        UINT64_C(0x400e000000000000)
    };
    static const uint64_t wheel_bits[3] = {
        UINT64_C(0xc010800000000000),
        UINT64_C(0x4016000000000000),
        UINT64_C(0xc01a800000000000)
    };
    DpVector torque_input = {3, 0, torque_input_data};
    DpVector torque_output = {3, 0, torque_output_data};
    DpVector wheel_input = {3, 0, wheel_input_data};
    DpVector wheel_output = {3, 0, wheel_output_data};

    L_c_B = (DpVector){3, 0, torque_global_data};
    H_w_B = (DpVector){3, 0, wheel_global_data};
    SetTorque(&torque_input);
    GetTorque(&torque_output);
    SetWheelAngularMoment(&wheel_input);
    GetWheelAngularMoment(&wheel_output);

    if (memcmp(torque_global_data, torque_bits, sizeof(torque_global_data)) != 0 ||
        memcmp(torque_output_data, torque_bits, sizeof(torque_output_data)) != 0 ||
        memcmp(wheel_global_data, wheel_bits, sizeof(wheel_global_data)) != 0 ||
        memcmp(wheel_output_data, wheel_bits, sizeof(wheel_output_data)) != 0 ||
        torque_output.count != 3 || torque_output.reserved_04 != 0 ||
        torque_output.data != torque_output_data || wheel_output.count != 3 ||
        wheel_output.reserved_04 != 0 || wheel_output.data != wheel_output_data) {
        (void)puts("torque/wheel-moment short ABI controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: Set/GetTorque and Set/GetWheelAngularMoment controlled original-ELF gold compare");
    return 0;
}

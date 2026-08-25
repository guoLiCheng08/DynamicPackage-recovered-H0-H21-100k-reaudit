#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_environment.h"
#include "dynamic_satellite_globals.h"

static int compare_bits(const char *label, const double actual[3],
                        const uint64_t expected[3])
{
    unsigned index;

    for (index = 0u; index < 3u; ++index) {
        uint64_t bits;

        memcpy(&bits, &actual[index], sizeof(bits));
        if (bits != expected[index]) {
            (void)printf("%s[%u] mismatch: got=%#llx expected=%#llx\n", label, index,
                         (unsigned long long)bits, (unsigned long long)expected[index]);
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    static const uint64_t expected_out[3] = {
        UINT64_C(0xc006b851eb851eb8), UINT64_C(0xbff6b851eb851eb8),
        UINT64_C(0x8000000000000000)
    };
    static const uint64_t expected_force[3] = {
        UINT64_C(0x3ff0000000000000), UINT64_C(0xc000000000000000),
        UINT64_C(0x4008000000000000)
    };
    static const uint64_t expected_velocity[3] = {
        UINT64_C(0x4010000000000000), UINT64_C(0xc014000000000000),
        UINT64_C(0x4018000000000000)
    };
    double output_data[3] = {0.0, 0.0, 0.0};
    double force_data[3] = {1.0, -2.0, 3.0};
    double velocity_data[3] = {4.0, -5.0, 6.0};
    double matrix_data[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    double expected_matrix[9];
    DpVector output = {3, 0, output_data};
    DpVector force = {3, 0, force_data};
    DpVector velocity = {3, 0, velocity_data};
    DpMatrix matrix = {3, 3, 3, 0, matrix_data};

    SatParaInit();
    memset(Sat.raw + 0x1070u, 0, sizeof(double));
    memcpy(expected_matrix, matrix_data, sizeof(matrix_data));
    AerodynamicTorque(&output, &force, &velocity, &matrix);
    if (compare_bits("AerodynamicTorque output", output_data, expected_out) != 0 ||
        compare_bits("AerodynamicTorque force", force_data, expected_force) != 0 ||
        compare_bits("AerodynamicTorque velocity", velocity_data, expected_velocity) != 0 ||
        memcmp(matrix_data, expected_matrix, sizeof(matrix_data)) != 0) {
        (void)puts("AerodynamicTorque observable ABI mismatch");
        return 1;
    }
    (void)puts("PASS: AerodynamicTorque controlled original-ELF gold compare");
    return 0;
}

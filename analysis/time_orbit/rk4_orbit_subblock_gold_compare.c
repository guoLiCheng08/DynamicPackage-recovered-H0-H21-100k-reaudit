#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"
#include "dynamic_recovered.h"
#include "dynamic_time.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static double from_bits(uint64_t value)
{
    double result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static void orbit_subblock_derivative(double time,
                                      const double state[DP_STATE_DIM],
                                      double derivative[DP_STATE_DIM],
                                      void *context)
{
    double position_raw[3];
    double acceleration_raw[3] = {0.0, 0.0, 0.0};
    DpVector position = {3, 0, position_raw};
    DpVector acceleration = {3, 0, acceleration_raw};
    unsigned index;

    (void)time;
    (void)context;
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        derivative[index] = 0.0;
    }
    position_raw[0] = state[7];
    position_raw[1] = state[8];
    position_raw[2] = state[9];
    orbit_dynamic(&acceleration, &position);
    derivative[7] = state[10];
    derivative[8] = state[11];
    derivative[9] = state[12];
    derivative[10] = acceleration_raw[0];
    derivative[11] = acceleration_raw[1];
    derivative[12] = acceleration_raw[2];
}

int main(void)
{
    const uint64_t initial[6] = {
        UINT64_C(0x41509592644cbdf4), UINT64_C(0x4154d396bd85c590),
        UINT64_C(0x411b7a6b485f560f), UINT64_C(0xc0b718923f395fb3),
        UINT64_C(0x40b23870c2afc513), UINT64_C(0x4082044582e51745)
    };
    const uint64_t expected[6] = {
        UINT64_C(0x415094fe92384cca), UINT64_C(0x4154d40b57e7035b),
        UINT64_C(0x411b7b51e2bf990b), UINT64_C(0xc0b7191423628791),
        UINT64_C(0x40b237cda007b205), UINT64_C(0x408203d986fb79df)
    };
    double state[DP_STATE_DIM] = {0.0};
    double time = 0.0;
    unsigned index;

    TimeInit(2020.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    SpacecraftMass = 100.0;
    F_I_external.data[0] = 0.0;
    F_I_external.data[1] = 0.0;
    F_I_external.data[2] = 0.0;
    for (index = 0u; index < 6u; ++index) {
        state[7u + index] = from_bits(initial[index]);
    }
    dp_rk4_step_33(state, &time, 0.1, orbit_subblock_derivative, NULL);
    if (bits(time) != bits(0.1)) {
        puts("RK4 orbit subblock: FAIL time");
        return 1;
    }
    for (index = 0u; index < 6u; ++index) {
        if (bits(state[7u + index]) != expected[index]) {
            printf("RK4 orbit subblock: FAIL field=%u actual=%a expected_bits=%016llx actual_bits=%016llx\n",
                   index, state[7u + index], (unsigned long long)expected[index],
                   (unsigned long long)bits(state[7u + index]));
            return 1;
        }
    }
    puts("RK4 orbit subblock original-ELF gold compare: PASS (bitwise)");
    return 0;
}

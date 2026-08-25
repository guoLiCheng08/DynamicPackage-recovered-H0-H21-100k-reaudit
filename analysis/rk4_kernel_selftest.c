#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_recovered.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static void constant_derivative(double time, const double state[DP_STATE_DIM],
                                double derivative[DP_STATE_DIM], void *context)
{
    const double *constant = context;
    unsigned index;

    (void)time;
    (void)state;
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        derivative[index] = constant[index];
    }
}

int main(void)
{
    double state[DP_STATE_DIM];
    double constant[DP_STATE_DIM];
    double initial[DP_STATE_DIM];
    double time = 2.0;
    unsigned index;

    for (index = 0u; index < DP_STATE_DIM; ++index) {
        state[index] = (double)index + 0.25;
        initial[index] = state[index];
        constant[index] = (double)(index + 1u) * 1.5;
    }
    dp_rk4_step_33(state, &time, 0.25, constant_derivative, constant);
    if (bits(time) != bits(2.25)) {
        puts("RK4 kernel self-test: FAIL time");
        return 1;
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        const double expected = initial[index] +
                                (double)(index + 1u) * 0.375;
        if (bits(state[index]) != bits(expected)) {
            printf("RK4 kernel self-test: FAIL index=%u actual=%a expected=%a\n",
                   index, state[index], expected);
            return 1;
        }
    }
    puts("RK4 33-state kernel self-test: PASS");
    return 0;
}

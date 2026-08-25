#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "dynamic_torque.h"
int main(void)
{
    DpVec3 moment = {2.0, -3.0, 5.0};
    DpVec3 field = {7.0, 11.0, -13.0};
    DpVec3 output = {0.0, 0.0, 0.0};
    const double expected[3] = {-16.0, 61.0, 43.0};
    uint64_t a, e;
    unsigned int i;
    if (MagTorque(&output, &moment, &field) != 0) return 1;
    for (i = 0; i < 3u; ++i) {
        memcpy(&a, ((double *)&output) + i, sizeof(a));
        memcpy(&e, expected + i, sizeof(e));
        if (a != e) return 1;
    }
    puts("MagTorque public ABI compare: PASS (bitwise)");
    return 0;
}

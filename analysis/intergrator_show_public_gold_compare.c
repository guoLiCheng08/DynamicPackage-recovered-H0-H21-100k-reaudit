#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

int main(void)
{
    static const double initial_y[DP_STATE_DIM] = {
        0.5, -0.5, 0.5, -0.5, 1.25, -2.5, 3.75,
        7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0,
        16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0,
        24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0
    };
    const double initial_t = -6.5;
    double expected_y[DP_STATE_DIM];
    double expected_t;

    memcpy(y, initial_y, sizeof(y));
    t = initial_t;
    memcpy(expected_y, y, sizeof(expected_y));
    expected_t = t;
    intergrator_show();
    if (memcmp(&t, &expected_t, sizeof(t)) != 0 ||
        memcmp(y, expected_y, sizeof(y)) != 0) {
        (void)puts("intergrator_show unexpectedly changed global t/y");
        return 1;
    }
    (void)puts("PASS: intergrator_show controlled original-ELF gold compare");
    return 0;
}

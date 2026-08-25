#include <stdio.h>

#include "dynamic_sensors.h"

int main(void)
{
    double position_data[3] = {7000000.0, 0.0, 0.0};
    double direction_data[3] = {-1.0, 0.0, 0.0};
    DpVector position = {3, 0, position_data};
    DpVector direction = {3, 0, direction_data};

    if (dp_is_earth_out(&position, &direction, 0.0) != 0) {
        puts("FAIL isEarthOut toward");
        return 1;
    }
    direction_data[0] = 1.0;
    if (dp_is_earth_out(&position, &direction, 0.0) != 1) {
        puts("FAIL isEarthOut away");
        return 1;
    }
    direction_data[0] = 0.0;
    direction_data[1] = 1.0;
    if (dp_is_earth_out(&position, &direction, 0.0) != 1) {
        puts("FAIL isEarthOut orthogonal");
        return 1;
    }
    puts("isEarthOut original-ELF gold compare: PASS");
    return 0;
}

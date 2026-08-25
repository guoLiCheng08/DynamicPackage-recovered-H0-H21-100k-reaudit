#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    double left_data[3] = {3.0, 4.0, 0.0};
    double right_data[3] = {0.0, 5.0, 0.0};
    DpVector left = {3, 0, left_data};
    DpVector right = {3, 0, right_data};
    double actual = vector2angle(&left, &right);

    if (bits(actual) != UINT64_C(0x3fe4978fa3269ee0)) {
        printf("FAIL vector2angle general actual=%a bits=%016" PRIx64 "\n", actual, bits(actual));
        return 1;
    }
    left_data[0] = 0.0;
    left_data[1] = 0.0;
    actual = vector2angle(&left, &right);
    if (bits(actual) != UINT64_C(0x0000000000000000)) {
        printf("FAIL vector2angle zero actual=%a bits=%016" PRIx64 "\n", actual, bits(actual));
        return 1;
    }
    puts("vector2angle original-ELF gold compare: PASS (bitwise)");
    return 0;
}

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_time.h"

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static int check_array(const double actual[6], const double expected[6],
                       const char *label)
{
    unsigned i;
    for (i = 0u; i < 6u; ++i) {
        if (bits(actual[i]) != bits(expected[i])) {
            printf("FAIL %s field=%u actual=%a expected=%a\n",
                   label, i, actual[i], expected[i]);
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    double actual[6];
    const double expected_negative[6] = {2024.0, 3.0, 1.0, 0.0, 0.0, -0.25};
    const double expected_carry[6] = {2024.0, 3.0, 1.0, 0.0, 0.0, 1.0};

    /* 原 ELF timeadd_boundary_gold.gdb 的连续金标。 */
    TimeInit(2024.0, 3.0, 1.0, 0.0, 0.0, 0.0);
    TimeAdd(-0.25);
    TimeArrayGet(actual);
    if (check_array(actual, expected_negative, "negative") != 0) return 1;
    if (bits(TimeTotalGet()) != bits(-0.25)) {
        puts("FAIL negative total");
        return 1;
    }

    TimeAdd(1.25);
    TimeArrayGet(actual);
    if (check_array(actual, expected_carry, "carry") != 0) return 1;
    if (bits(TimeTotalGet()) != bits(1.0)) {
        puts("FAIL carry total");
        return 1;
    }

    puts("TimeAdd boundary original-ELF gold compare: PASS (bitwise)");
    return 0;
}

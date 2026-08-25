#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_environment.h"

#define GOLD_PATH "analysis/time_orbit/gold_inertial_magnetic_step2.bin"

static int read_gold(double gold[15])
{
    FILE *file = fopen(GOLD_PATH, "rb");
    if (file == NULL) {
        perror(GOLD_PATH);
        return -1;
    }
    if (fread(gold, sizeof(gold[0]), 15u, file) != 15u || fclose(file) != 0) {
        return -1;
    }
    return 0;
}

int main(void)
{
    double gold[15];
    double actual_data[3] = {0.0};
    DpCalendarTime calendar;
    DpVector position;
    DpVector velocity;
    DpVector actual = {3, 0, actual_data};
    unsigned index;

    if (read_gold(gold) != 0) return 1;
    memcpy(&calendar, gold, sizeof(calendar));
    position.count = 3;
    position.reserved_04 = 0;
    position.data = gold + 6;
    velocity.count = 3;
    velocity.reserved_04 = 0;
    velocity.data = gold + 9;
    dp_calc_inertial_magnetic_vector(&actual, &position, &velocity, &calendar);
    for (index = 0u; index < 3u; ++index) {
        uint64_t actual_bits;
        uint64_t expected_bits;
        memcpy(&actual_bits, &actual_data[index], sizeof(actual_bits));
        memcpy(&expected_bits, &gold[index + 12u], sizeof(expected_bits));
        if (actual_bits != expected_bits) {
            fprintf(stderr,
                    "Calc_InertialMagneticVector[%u] got=%016llx expected=%016llx\n",
                    index, (unsigned long long)actual_bits,
                    (unsigned long long)expected_bits);
            return 1;
        }
    }
    puts("Calc_InertialMagneticVector original-ELF gold compare: PASS (bitwise)");
    return 0;
}

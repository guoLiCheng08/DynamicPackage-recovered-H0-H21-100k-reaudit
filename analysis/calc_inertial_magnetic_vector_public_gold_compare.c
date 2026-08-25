/* Calc_InertialMagneticVector 公共 ABI：复用原 ELF 惯性磁场步进金标。 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_environment.h"

#define GOLD_PATH "analysis/time_orbit/gold_inertial_magnetic_step2.bin"

static int read_gold(double gold[15])
{
    FILE *file = fopen(GOLD_PATH, "rb");

    if (file == NULL) {
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
    double actual_data[3] = {0.0, 0.0, 0.0};
    DpCalendarTime calendar;
    DpVector position;
    DpVector velocity;
    DpVector actual = {3, 0, actual_data};
    unsigned int index;

    if (read_gold(gold) != 0) {
        return 2;
    }
    memcpy(&calendar, gold, sizeof(calendar));
    position.count = 3;
    position.reserved_04 = 0;
    position.data = gold + 6;
    velocity.count = 3;
    velocity.reserved_04 = 0;
    velocity.data = gold + 9;
    Calc_InertialMagneticVector(&actual, &position, &velocity, &calendar);
    for (index = 0u; index < 3u; ++index) {
        uint64_t actual_bits;
        uint64_t expected_bits;
        memcpy(&actual_bits, &actual_data[index], sizeof(actual_bits));
        memcpy(&expected_bits, &gold[index + 12u], sizeof(expected_bits));
        if (actual_bits != expected_bits) {
            (void)fprintf(stderr,
                          "Calc_InertialMagneticVector[%u] got=%016llx expected=%016llx\n",
                          index, (unsigned long long)actual_bits,
                          (unsigned long long)expected_bits);
            return 1;
        }
    }
    (void)puts("Calc_InertialMagneticVector public ABI original-ELF gold compare: PASS (bitwise)");
    return 0;
}

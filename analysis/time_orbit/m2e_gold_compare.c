#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"

typedef struct {
    double mean_anomaly;
    double eccentricity;
    double expected;
} M2ECase;

static uint64_t bits(double value)
{
    uint64_t result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

int main(void)
{
    /* 由隔离 GDB 在原 ELF 的 M2E 函数上直接调用并记录。 */
    const M2ECase cases[] = {
        {0.0, 0.0, 0.0},
        {1.0, 0.1, 1.0885977523978936},
        {2.2, 0.65, 2.5581080006663384},
        {-1.7, 0.25, -1.9337160712926771},
    };
    double expected_gold[4];
    FILE *gold_file;
    unsigned i;

    gold_file = fopen("analysis/time_orbit/m2e_gold.bin", "rb");
    if (gold_file == NULL || fread(expected_gold, 1u, sizeof(expected_gold), gold_file) != sizeof(expected_gold) ||
        fgetc(gold_file) != EOF || fclose(gold_file) != 0) return 2;
    for (i = 0u; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const double actual = M2E(cases[i].mean_anomaly, cases[i].eccentricity);
        if (bits(actual) != bits(cases[i].expected) || bits(actual) != bits(expected_gold[i])) {
            printf("FAIL case=%u actual=%a expected=%a actual_bits=%016llx expected_bits=%016llx\n",
                   i, actual, cases[i].expected,
                   (unsigned long long)bits(actual),
                   (unsigned long long)bits(cases[i].expected));
            return 1;
        }
    }
    puts("M2E original-ELF gold compare: PASS (bitwise)");
    return 0;
}

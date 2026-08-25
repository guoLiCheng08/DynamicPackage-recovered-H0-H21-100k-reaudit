#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"

static int expect_bits(double value, uint64_t expected, const char *label)
{
    uint64_t actual;

    memcpy(&actual, &value, sizeof(actual));
    if (actual != expected) {
        (void)printf("%s actual=%#llx expected=%#llx\n", label,
                     (unsigned long long)actual, (unsigned long long)expected);
        return -1;
    }
    return 0;
}

int main(void)
{
    SetSpacecraftMass(123.75);
    if (expect_bits(SpacecraftMass, UINT64_C(0x405ef00000000000), "mass A") != 0) {
        return 1;
    }
    SetSpacecraftMass(0.0);
    if (expect_bits(SpacecraftMass, UINT64_C(0x0000000000000000), "mass B") != 0) {
        return 1;
    }
    (void)puts("PASS: SetSpacecraftMass controlled original-ELF gold compare");
    return 0;
}

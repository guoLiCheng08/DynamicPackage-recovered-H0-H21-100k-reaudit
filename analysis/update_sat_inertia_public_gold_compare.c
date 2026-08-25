#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

static const uint64_t flag0_j_bits[9] = {
    UINT64_C(0x405e000000000000), UINT64_C(0xbfe428f5c28f5c29),
    UINT64_C(0x3ff4cccccccccccd), UINT64_C(0xbfe428f5c28f5c29),
    UINT64_C(0x4076900000000000), UINT64_C(0xbfe0000000000000),
    UINT64_C(0x3ff4cccccccccccd), UINT64_C(0xbfe0000000000000),
    UINT64_C(0x407d500000000000)
};
static const uint64_t flag0_jinv_bits[9] = {
    UINT64_C(0x3f81113cdb6f56f3), UINT64_C(0x3eee6ed1eab39b43),
    UINT64_C(0xbef8349fe3dcda19), UINT64_C(0x3eee6ed1eab39b43),
    UINT64_C(0x3f66b158cdc4be55), UINT64_C(0x3ec86f97ee84395f),
    UINT64_C(0xbef8349fe3dcda19), UINT64_C(0x3ec86f97ee84395f),
    UINT64_C(0x3f6177ae1e1da471)
};
static const uint64_t flag1_j_bits[9] = {
    UINT64_C(0x409e840000000000), UINT64_C(0xbfe0000000000000),
    UINT64_C(0x3ff7ae147ae147ae), UINT64_C(0xbfe0000000000000),
    UINT64_C(0x4081000000000000), UINT64_C(0xc031b33333333333),
    UINT64_C(0x3ff7ae147ae147ae), UINT64_C(0xc031b33333333333),
    UINT64_C(0x40a02e0000000000)
};
static const uint64_t flag1_jinv_bits[9] = {
    UINT64_C(0x3f40c73ed7a6f404), UINT64_C(0x3e9ecad2713f4351),
    UINT64_C(0xbe984afe41ee56fd), UINT64_C(0x3e9ecad2713f4351),
    UINT64_C(0x3f5e2043949fbb49), UINT64_C(0x3ef07a61f1bac58b),
    UINT64_C(0xbe984afe41ee56fd), UINT64_C(0x3ef07a61f1bac58b),
    UINT64_C(0x3f3fa74784c1a1aa)
};
static const uint64_t default_j_bits[9] = {
    UINT64_C(0x4000000000000000), UINT64_C(0), UINT64_C(0),
    UINT64_C(0), UINT64_C(0x4008000000000000), UINT64_C(0),
    UINT64_C(0), UINT64_C(0), UINT64_C(0x4014000000000000)
};
static const uint64_t default_jinv_bits[9] = {
    UINT64_C(0x3fe0000000000000), UINT64_C(0), UINT64_C(0),
    UINT64_C(0), UINT64_C(0x3fd5555555555555), UINT64_C(0),
    UINT64_C(0), UINT64_C(0), UINT64_C(0x3fc999999999999a)
};

static int compare_bits(const char *label, const double *actual, const uint64_t expected[9])
{
    size_t index;

    for (index = 0u; index < 9u; ++index) {
        uint64_t actual_bits;

        memcpy(&actual_bits, &actual[index], sizeof(actual_bits));
        if (actual_bits != expected[index]) {
            (void)printf("%s[%zu] mismatch: got=%#llx expected=%#llx\n", label, index,
                         (unsigned long long)actual_bits, (unsigned long long)expected[index]);
            return 1;
        }
    }
    return 0;
}

static int compare_case(const char *label, const uint64_t j_bits[9], const uint64_t jinv_bits[9])
{
    DpMatrix sat_j;
    DpMatrix sat_jinv;

    memcpy(&sat_j, &Sat.raw[0x08u], sizeof(sat_j));
    memcpy(&sat_jinv, &Sat.raw[0x68u], sizeof(sat_jinv));
    return compare_bits(label, sat_j.data, j_bits) ||
           compare_bits(label, sat_jinv.data, jinv_bits) ||
           compare_bits(label, J_c_B.data, j_bits) ||
           compare_bits(label, J_c_B_inv.data, jinv_bits);
}

int main(void)
{
    DpMatrix sat_j;
    size_t index;

    memset(&Sat, 0, sizeof(Sat));
    SatParaInit();
    Update_sat_inertia(0);
    if (compare_case("Update_sat_inertia(flag=0)", flag0_j_bits, flag0_jinv_bits) != 0) return 1;
    Update_sat_inertia(1);
    if (compare_case("Update_sat_inertia(flag=1)", flag1_j_bits, flag1_jinv_bits) != 0) return 1;
    memcpy(&sat_j, &Sat.raw[0x08u], sizeof(sat_j));
    for (index = 0u; index < 9u; ++index) {
        memcpy(&sat_j.data[index], &default_j_bits[index], sizeof(sat_j.data[index]));
    }
    Update_sat_inertia(2);
    if (compare_case("Update_sat_inertia(flag=2)", default_j_bits, default_jinv_bits) != 0) return 1;
    (void)puts("PASS: Update_sat_inertia controlled original-ELF gold compare");
    return 0;
}

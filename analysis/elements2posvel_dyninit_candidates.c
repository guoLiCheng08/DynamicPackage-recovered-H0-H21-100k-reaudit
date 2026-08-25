#define _GNU_SOURCE
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_orbit.h"

static uint64_t bits(double value)
{
    uint64_t out;
    memcpy(&out, &value, sizeof(out));
    return out;
}

static double triple(double a, double b, double c, unsigned order)
{
    return order == 0u ? (a * b) * c : a * (b * c);
}

int main(void)
{
    const double a = 7000000.0, e = 0.01, inc = 0.5, raan = 0.2, arg = 0.3, m = 0.4;
    const double mu = DP_ORBIT_EARTH_MODEL.gravitational_parameter;
    const uint64_t expected[3] = {
        UINT64_C(0xc0b6d315f6591ac8), UINT64_C(0x40af5bd7fe7f9f8e),
        UINT64_C(0x40a5be8d54767228)
    };
    double E = M2E(m, e), se, ce, si, ci, sr, cr, sa, ca;
    double sq, n, denom, factor, vx, vy;
    unsigned mask;
    unsigned vy_order;

    printf("recovered_M2E_bits=%016llx\n", (unsigned long long)bits(E));
    sincos(E, &se, &ce);
    sincos(inc, &si, &ci);
    sincos(raan, &sr, &cr);
    sincos(arg, &sa, &ca);
    sq = sqrt(1.0 - e * e);
    n = sqrt(mu / (a * a * a));
    denom = 1.0 - e * ce;
    printf("recovered_n_bits=%016llx\n", (unsigned long long)bits(n));
    printf("recovered_denom_bits=%016llx\n", (unsigned long long)bits(denom));
    factor = a * n / denom;
    printf("recovered_velocity_factor_bits=%016llx\n", (unsigned long long)bits(factor));
    vx = -factor * se;
    for (vy_order = 0u; vy_order < 3u; ++vy_order) {
        if (vy_order == 0u) vy = (sq * factor) * ce;
        else if (vy_order == 1u) vy = sq * (factor * ce);
        else vy = (sq * ce) * factor;
    for (mask = 0u; mask < 16u; ++mask) {
        double r11 = cr * ca - triple(sr, sa, ci, mask & 1u);
        double r12 = -cr * sa - triple(sr, ca, ci, (mask >> 1u) & 1u);
        double r21 = sr * ca + triple(cr, sa, ci, (mask >> 2u) & 1u);
        double r22 = -sr * sa + triple(cr, ca, ci, (mask >> 3u) & 1u);
        double out[3];
        out[0] = r11 * vx + r12 * vy;
        out[1] = r21 * vx + r22 * vy;
        out[2] = (sa * si) * vx + (ca * si) * vy;
        if (bits(out[0]) == expected[0] && bits(out[1]) == expected[1] &&
            bits(out[2]) == expected[2]) {
            printf("vy_order%u rotation association mask %u MATCH\n", vy_order, mask);
        }
        if (mask == 0u || mask == 15u) {
            printf("vy_order%u mask%u: %016llx %016llx %016llx\n", vy_order, mask,
                   (unsigned long long)bits(out[0]), (unsigned long long)bits(out[1]),
                   (unsigned long long)bits(out[2]));
        }
    }
    }
    return 0;
}

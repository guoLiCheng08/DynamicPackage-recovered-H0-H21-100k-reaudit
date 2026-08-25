#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

static int compare_file(const char *name, const double *actual)
{
    double expected[12];
    FILE *file;
    unsigned index;

    file = fopen(name, "rb");
    if (file == NULL || fread(expected, 1u, sizeof(expected), file) != sizeof(expected) ||
        fclose(file) != 0) {
        return 2;
    }
    for (index = 0u; index < 12u; ++index) {
        uint64_t a;
        uint64_t e;

        memcpy(&a, &actual[index], sizeof(a));
        memcpy(&e, &expected[index], sizeof(e));
        if (a != e) {
            printf("%s[%u] actual=%016" PRIx64 " expected=%016" PRIx64 "\n",
                   name, index, a, e);
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    uint64_t neg_nan_bits = UINT64_C(0xfff8000000000000);
    double q_xyz[3];
    DpQuatAbi quat;
    double psi_data[12] = {0.0};
    double xi_data[12] = {0.0};
    DpMatrix psi = {4, 3, 3, 0, psi_data};
    DpMatrix xi = {4, 3, 3, 0, xi_data};
    unsigned index;

    memcpy(&quat.w, &neg_nan_bits, sizeof(quat.w));
    for (index = 0u; index < 3u; ++index) {
        memcpy(&q_xyz[index], &neg_nan_bits, sizeof(q_xyz[index]));
    }
    quat.xyz.count = 3;
    quat.xyz.reserved_04 = 0;
    quat.xyz.data = q_xyz;
    quat_psi(&psi, &quat);
    quat_xi(&xi, &quat);
    if (compare_file("analysis/time_orbit/gold_quat_psi_h0_step101_neg_nan.bin", psi_data) != 0 ||
        compare_file("analysis/time_orbit/gold_quat_xi_h0_step101_neg_nan.bin", xi_data) != 0) {
        return 1;
    }
    puts("quat_psi/quat_xi H0 step101 neg-NaN original-ELF compare: PASS (bitwise)");
    return 0;
}

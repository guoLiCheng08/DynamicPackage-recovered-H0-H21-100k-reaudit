/* GravityGradientTorque 公共 ABI：复用已验证的原 ELF core-dynamic 步进金标。 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_torque.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        return -1;
    }
    if (fread(out, 1u, bytes, file) != bytes || fclose(file) != 0) {
        return -1;
    }
    return 0;
}

int main(void)
{
    uint8_t satellite[0x1078];
    uint8_t torque_block[0x168];
    DpMatrix inertia = {3, 3, 0, 0, NULL};
    DpMatrix attitude = {3, 3, 3, 0, NULL};
    DpVec3 position;
    DpVec3 actual;
    DpVec3 expected;
    unsigned int index;

    if (read_blob("gold_core_dynamic_sat_step2.bin", satellite, sizeof(satellite)) != 0 ||
        read_blob("gold_core_dynamic_torque_step2.bin", torque_block, sizeof(torque_block)) != 0) {
        return 2;
    }
    inertia.data = (double *)(satellite + 0x20u);
    attitude.data = (double *)(satellite + 0x1c0u);
    memcpy(&position, satellite + 0x128u, sizeof(position));
    memcpy(&expected, torque_block + 0x0c8u, sizeof(expected));
    if (GravityGradientTorque(&actual, &position, &inertia, &attitude) != 0) {
        return 1;
    }
    for (index = 0u; index < 3u; ++index) {
        uint64_t observed_bits;
        uint64_t expected_bits;
        memcpy(&observed_bits, ((const double *)&actual) + index, sizeof(observed_bits));
        memcpy(&expected_bits, ((const double *)&expected) + index, sizeof(expected_bits));
        if (observed_bits != expected_bits) {
            (void)fprintf(stderr, "GravityGradientTorque[%u] got=%016llx expected=%016llx\n",
                          index, (unsigned long long)observed_bits,
                          (unsigned long long)expected_bits);
            return 1;
        }
    }
    (void)puts("GravityGradientTorque public ABI original-ELF gold compare: PASS (bitwise)");
    return 0;
}

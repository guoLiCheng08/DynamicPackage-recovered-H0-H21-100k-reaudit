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
    if (file == NULL) { perror(path); return -1; }
    if (fread(out, 1u, bytes, file) != bytes || fclose(file) != 0) return -1;
    return 0;
}

int main(void)
{
    uint8_t sat[0x1078];
    uint8_t torque[0x168];
    DpMatrix inertia = {3, 3, 0, 0, NULL};
    DpMatrix attitude = {3, 3, 3, 0, NULL};
    DpVec3 position;
    DpVec3 actual;
    DpVec3 expected;
    unsigned index;

    if (read_blob("gold_core_dynamic_sat_step2.bin", sat, sizeof(sat)) != 0 ||
        read_blob("gold_core_dynamic_torque_step2.bin", torque, sizeof(torque)) != 0) return 1;
    inertia.data = (double *)(sat + 0x20u);
    attitude.data = (double *)(sat + 0x1c0u);
    memcpy(&position, sat + 0x128u, sizeof(position));
    memcpy(&expected, torque + 0x0c8u, sizeof(expected));
    if (dp_gravity_gradient_torque(&actual, &position, &attitude, &inertia) != 0) return 1;
    for (index = 0u; index < 3u; ++index) {
        uint64_t a;
        uint64_t e;
        memcpy(&a, ((const double *)&actual) + index, sizeof(a));
        memcpy(&e, ((const double *)&expected) + index, sizeof(e));
        if (a != e) {
            fprintf(stderr, "gravity[%u] got=%016llx expected=%016llx\n", index,
                    (unsigned long long)a, (unsigned long long)e);
            return 1;
        }
    }
    puts("GravityGradientTorque original-ELF gold compare: PASS (bitwise)");
    return 0;
}

#include "dynamic_recovered.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_exact(const char *path, void *dst, size_t n)
{
    FILE *fp = fopen(path, "rb");
    int ok = 0;
    if (fp != NULL) {
        ok = fread(dst, 1, n, fp) == n && fgetc(fp) == EOF;
        (void)fclose(fp);
    }
    return ok;
}

static void show_double(const char *name, double value)
{
    uint64_t bits = 0;
    (void)memcpy(&bits, &value, sizeof(bits));
    (void)printf("%s = %.17g (0x%016" PRIx64 ")\n", name, value, bits);
}

static void show_float(const char *name, float value)
{
    uint32_t bits = 0;
    (void)memcpy(&bits, &value, sizeof(bits));
    (void)printf("%s = %.9g (0x%08" PRIx32 ")\n", name, value, bits);
}

int main(void)
{
    const char *path = "analysis/coverage_inventory/target_closure/gold_dynamic_dll_init_p0_init.bin";
    DpInitialConditions init;
    size_t i;
    char name[64];

    if (!read_exact(path, &init, sizeof(init))) {
        (void)fprintf(stderr, "cannot read %s\n", path);
        return 2;
    }
    show_double("step_time", init.step_time);
    show_double("initial_time_or_epoch", init.initial_time_or_epoch);
    show_double("initial_attitude_vector.x", init.initial_attitude_vector.x);
    show_double("initial_attitude_vector.y", init.initial_attitude_vector.y);
    show_double("initial_attitude_vector.z", init.initial_attitude_vector.z);
    for (i = 0; i < 3; ++i) {
        (void)snprintf(name, sizeof(name), "initial_angular_rate_f32[%zu]", i);
        show_float(name, init.initial_angular_rate_f32[i]);
    }
    (void)printf("reserved_34 = 0x%08" PRIx32 "\n", init.reserved_34);
    for (i = 0; i < 6; ++i) {
        (void)snprintf(name, sizeof(name), "time_parameters[%zu]", i);
        show_double(name, init.time_parameters[i]);
    }
    for (i = 0; i < 6; ++i) {
        (void)snprintf(name, sizeof(name), "orbit_elements[%zu]", i);
        show_double(name, init.orbit_elements[i]);
    }
    for (i = 0; i < 9; ++i) {
        (void)snprintf(name, sizeof(name), "inertia_tensor[%zu]", i);
        show_double(name, init.inertia_tensor[i]);
    }
    show_double("spacecraft_mass", init.spacecraft_mass);
    return 0;
}

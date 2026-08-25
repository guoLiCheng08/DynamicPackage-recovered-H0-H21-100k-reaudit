#include <stdint.h>
#include <stdio.h>

#include "dynamic_core_environment.h"
#include "dynamic_core_bridge.h"
#include "dynamic_recovered.h"
#include "dynamic_time.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) return -1;
    if (fread(out, 1u, bytes, file) != bytes || fclose(file) != 0) return -1;
    return 0;
}

int main(void)
{
    DpCoreDefaultModel model;
    DpState actual;
    DpState expected;
    size_t index;

    if (read_blob("gold_sada_position_step1_post_y.bin", &actual, sizeof(actual)) != 0 ||
        read_blob("gold_sada_position_step2_post_y.bin", &expected, sizeof(expected)) != 0 ||
        dp_core_default_model_init(&model) != 0) return 1;
    model.sada_drive.command_angle[0] = 0.01;
    model.sada_drive.command_angle[1] = -0.01;
    model.sada_drive.angular_acceleration[0] = 1.7453292519943296e-05;
    model.sada_drive.angular_acceleration[1] = -8.726646259971648e-06;
    TimeInit(2025.0, 1.0, 2.0, 3.0, 4.0, 5.1);
    SpacecraftMass = 1000.0;
    F_I_external.data[0] = 0.0;
    F_I_external.data[1] = 0.0;
    F_I_external.data[2] = 0.0;
    if (dp_core_default_rk4_step_with_terms_and_environment(&model, (double *)&actual,
            &(double){0.1}, 0.1, NULL, NULL, &(DpCoreEnvironmentOutputs){0}) != 0) return 1;
    for (index = 0u; index < sizeof(actual); ++index) {
        if (((const uint8_t *)&actual)[index] != ((const uint8_t *)&expected)[index]) {
            fprintf(stderr, "sada_position core state +0x%zx got=%02x expected=%02x\n", index,
                    (unsigned)((const uint8_t *)&actual)[index],
                    (unsigned)((const uint8_t *)&expected)[index]);
            return 1;
        }
    }
    puts("sada_position CoreDynamic second-step compare: PASS (bitwise)");
    return 0;
}

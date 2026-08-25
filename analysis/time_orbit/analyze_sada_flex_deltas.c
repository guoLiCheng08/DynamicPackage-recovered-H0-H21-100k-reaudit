#include <stdio.h>

#define STAGES 4u
#define VALUES 13u

static int read_values(const char *path, double *out, size_t count)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return -1;
    if (fread(out, sizeof(double), count, file) != count || fclose(file) != 0) return -1;
    return 0;
}

int main(void)
{
    double original[STAGES][VALUES];
    double recovered[STAGES][VALUES];
    unsigned stage;
    unsigned index;
    char path[256];

    for (stage = 0u; stage < STAGES; ++stage) {
        (void)snprintf(path, sizeof(path),
                       "analysis/time_orbit/gold_sada_position_step1_flex_stage%u.bin", stage + 1u);
        if (read_values(path, original[stage], VALUES) != 0) return 1;
    }
    if (read_values("/tmp/e10_recovered_sada_base_stages.bin", &recovered[0][0],
                    STAGES * VALUES) != 0) return 1;

    for (stage = 0u; stage < STAGES; ++stage) {
        printf("stage %u delta [alpha; eta_ddot]:\n", stage + 1u);
        for (index = 0u; index < VALUES; ++index) {
            printf("% .17g%s", original[stage][index] - recovered[stage][index],
                   index + 1u == VALUES ? "\n" : " ");
        }
    }
    return 0;
}

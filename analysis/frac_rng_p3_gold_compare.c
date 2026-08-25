#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_math.h"

#define FRAC_GOLD "analysis/time_orbit/frac_gold.bin"
#define RNG_GOLD "analysis/time_orbit/gold_rng_uniform_pos_p3_output.bin"

static int read_gold(const char *path, void *out, size_t bytes)
{
    FILE *file = fopen(path, "rb");
    size_t count;
    if (file == NULL) { perror(path); return -1; }
    count = fread(out, 1u, bytes, file);
    return fclose(file) == 0 && count == bytes ? 0 : -1;
}

static int compare(const char *label, const void *actual, const void *expected, size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;
    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            fprintf(stderr, "%s mismatch +0x%zx: actual=%02x expected=%02x\n", label,
                    index, (unsigned)a[index], (unsigned)e[index]);
            return 1;
        }
    }
    fprintf(stderr, "%s mismatched bytes: 0/%zu\n", label, bytes);
    return 0;
}

int main(void)
{
    double inputs[6] = {1.25, -1.25, 2.0, -2.0, 4503599627370496.0, 0.0};
    double actual_frac[6];
    double actual_rng[4];
    unsigned char expected_frac[sizeof(actual_frac)];
    unsigned char expected_rng[sizeof(actual_rng)];
    unsigned index;
    int mismatch;

    ((unsigned long long *)inputs)[5] = 0x8000000000000000ULL;
    if (read_gold(FRAC_GOLD, expected_frac, sizeof(expected_frac)) != 0 ||
        read_gold(RNG_GOLD, expected_rng, sizeof(expected_rng)) != 0) return 1;
    for (index = 0u; index < 6u; ++index) actual_frac[index] = Frac(inputs[index]);
    srand(1u);
    for (index = 0u; index < 4u; ++index) actual_rng[index] = rng_uniform_pos();
    mismatch = compare("Frac P3", actual_frac, expected_frac, sizeof(actual_frac));
    mismatch |= compare("rng_uniform_pos P3", actual_rng, expected_rng, sizeof(actual_rng));
    if (mismatch == 0) puts("Frac/rng_uniform_pos P3 original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_exact(const char *name, void *out, size_t bytes)
{
    char path[256]; FILE *file; size_t count;
    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) { perror(path); return -1; }
    count = fread(out, 1u, bytes, file);
    return fclose(file) == 0 && count == bytes ? 0 : -1;
}

static int compare_blob(const char *label, const unsigned char *actual,
                        const unsigned char *expected, size_t bytes)
{
    size_t index; unsigned mismatches = 0u;
    for (index = 0u; index < bytes; ++index) if (actual[index] != expected[index]) ++mismatches;
    printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

int main(void)
{
    unsigned char expected_results[8];
    unsigned char actual_results[8] = {0};
    double expected_snapshots[6];
    double actual_snapshots[6];
    double data[3] = {1.5, -2.0, 0.0};
    DpVector vector = {0, 0, data};
    int result;
    int mismatch = 0;

    if (read_exact("gold_vector_add_constant_nonfatal_p3_results.bin", expected_results,
                   sizeof(expected_results)) != 0 ||
        read_exact("gold_vector_add_constant_nonfatal_p3_snapshots.bin", expected_snapshots,
                   sizeof(expected_snapshots)) != 0) return 1;
    result = vector_add_constant(&vector, -1.5);
    memcpy(actual_results, &result, sizeof(result));
    memcpy(actual_snapshots, data, sizeof(data));
    vector.count = 3;
    result = vector_add_constant(&vector, -1.5);
    memcpy(actual_results + 4u, &result, sizeof(result));
    memcpy(actual_snapshots + 3, data, sizeof(data));
    mismatch |= compare_blob("vector_add_constant P3 returns", actual_results, expected_results,
                             sizeof(actual_results));
    mismatch |= compare_blob("vector_add_constant P3 snapshots", (const unsigned char *)actual_snapshots,
                             (const unsigned char *)expected_snapshots, sizeof(actual_snapshots));
    if (mismatch == 0) puts("vector_add_constant nonfatal P3 original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

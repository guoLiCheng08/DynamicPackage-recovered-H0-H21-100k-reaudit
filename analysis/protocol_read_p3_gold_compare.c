#include <stdio.h>
#include <string.h>

#include "dynamic_ipc_telemetry.h"

#define FLOAT_GOLD "analysis/time_orbit/read_float32_gold.bin"
#define LONG_GOLD "analysis/time_orbit/read_long_gold.bin"
#define UINT8_GOLD "analysis/time_orbit/read_uint8_gold.bin"

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
    const uint8_t source[4] = {0x12u, 0x34u, 0x56u, 0x78u};
    float actual_float[2];
    unsigned long actual_long[2];
    uint8_t actual_uint8;
    unsigned char expected_float[sizeof(actual_float)];
    unsigned char expected_long[sizeof(actual_long)];
    unsigned char expected_uint8;
    int mismatch;

    if (read_gold(FLOAT_GOLD, expected_float, sizeof(expected_float)) != 0 ||
        read_gold(LONG_GOLD, expected_long, sizeof(expected_long)) != 0 ||
        read_gold(UINT8_GOLD, &expected_uint8, sizeof(expected_uint8)) != 0) return 1;
    actual_float[0] = read_float32(source, 0);
    actual_float[1] = read_float32(source, 1);
    actual_long[0] = (unsigned long)read_long(source, 0);
    actual_long[1] = (unsigned long)read_long(source, 1);
    actual_uint8 = read_Uint8(source);
    mismatch = compare("protocol read P3 float32", actual_float, expected_float,
                       sizeof(actual_float));
    mismatch |= compare("protocol read P3 long", actual_long, expected_long,
                        sizeof(actual_long));
    mismatch |= compare("protocol read P3 uint8", &actual_uint8, &expected_uint8,
                        sizeof(actual_uint8));
    if (mismatch == 0) puts("protocol read P3 original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

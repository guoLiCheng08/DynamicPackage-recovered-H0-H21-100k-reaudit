#include <stdio.h>
#include <string.h>

#include "dynamic_ipc_telemetry.h"

static int load_gold(const char *path, unsigned char output[5])
{
    FILE *file = fopen(path, "rb");
    size_t bytes;

    if (file == NULL) {
        (void)fprintf(stderr, "cannot open gold: %s\n", path);
        return 1;
    }
    bytes = fread(output, 1u, 5u, file);
    if (fclose(file) != 0 || bytes != 5u) {
        (void)fprintf(stderr, "invalid gold: %s\n", path);
        return 1;
    }
    return 0;
}

int main(void)
{
    const unsigned char source[5] = {0x12u, 0x34u, 0x56u, 0x78u, 0x9au};
    unsigned char expected_native[5];
    unsigned char expected_reverse[5];
    unsigned char expected_other[5];
    unsigned char native[5];
    unsigned char reverse[5];
    unsigned char other[5];

    if (load_gold("analysis/time_orbit/var2buff_gold_native.bin", expected_native) != 0 ||
        load_gold("analysis/time_orbit/var2buff_gold_reverse.bin", expected_reverse) != 0 ||
        load_gold("analysis/time_orbit/var2buff_gold_other.bin", expected_other) != 0) {
        return 1;
    }
    memset(native, 0xa5, sizeof(native));
    memset(reverse, 0xa5, sizeof(reverse));
    memset(other, 0xa5, sizeof(other));
    var2buff(native, source, 5, 1u);
    var2buff(reverse, source, 5, 0u);
    var2buff(other, source, 5, 2u);
    if (memcmp(native, expected_native, sizeof(native)) != 0 ||
        memcmp(reverse, expected_reverse, sizeof(reverse)) != 0 ||
        memcmp(other, expected_other, sizeof(other)) != 0) {
        (void)puts("var2buff original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: var2buff controlled original-ELF gold compare");
    return 0;
}

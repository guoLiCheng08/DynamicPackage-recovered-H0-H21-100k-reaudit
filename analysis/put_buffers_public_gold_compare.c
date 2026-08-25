#include <stdio.h>
#include <string.h>

#include "dynamic_ipc_telemetry.h"

static int load_gold(const char *path, void *destination, size_t bytes)
{
    FILE *file = fopen(path, "rb");
    size_t count;

    if (file == NULL) {
        (void)fprintf(stderr, "cannot open gold: %s\n", path);
        return 1;
    }
    count = fread(destination, 1u, bytes, file);
    if (fclose(file) != 0 || count != bytes) {
        (void)fprintf(stderr, "invalid gold: %s\n", path);
        return 1;
    }
    return 0;
}

int main(void)
{
    unsigned char u16[2];
    unsigned char u32[4];
    unsigned char f32[4];
    unsigned char f64[8];
    unsigned char expected_u16[2];
    unsigned char expected_u32[4];
    unsigned char expected_f32[4];
    unsigned char expected_f64[8];

    if (load_gold("analysis/time_orbit/putuint16inbuff_gold.bin", expected_u16, sizeof(expected_u16)) != 0 ||
        load_gold("analysis/time_orbit/putuint32inbuff_gold.bin", expected_u32, sizeof(expected_u32)) != 0 ||
        load_gold("analysis/time_orbit/putfloat32inbuff_gold.bin", expected_f32, sizeof(expected_f32)) != 0 ||
        load_gold("analysis/time_orbit/putfloat64inbuff_gold.bin", expected_f64, sizeof(expected_f64)) != 0) {
        return 1;
    }
    memset(u16, 0xa5, sizeof(u16));
    memset(u32, 0xa5, sizeof(u32));
    memset(f32, 0xa5, sizeof(f32));
    memset(f64, 0xa5, sizeof(f64));
    Put_UINT16_In_Buff(u16, UINT16_C(0x1234), 0u);
    Put_UINT32_In_Buff(u32, UINT32_C(0x12345678), 1u);
    Put_FLOAT32_In_Buff(f32, 0u, 1.25f);
    Put_FLOAT64_In_Buff(f64, 1u, -2.5);
    if (memcmp(u16, expected_u16, sizeof(u16)) != 0 ||
        memcmp(u32, expected_u32, sizeof(u32)) != 0 ||
        memcmp(f32, expected_f32, sizeof(f32)) != 0 ||
        memcmp(f64, expected_f64, sizeof(f64)) != 0) {
        (void)puts("Put_*_In_Buff original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: Put_UINT16_In_Buff / Put_UINT32_In_Buff / Put_FLOAT32_In_Buff / Put_FLOAT64_In_Buff controlled original-ELF gold compare");
    return 0;
}

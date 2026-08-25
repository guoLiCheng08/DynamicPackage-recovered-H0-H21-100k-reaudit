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
    const uint8_t u8_source[1] = {0x12u};
    const uint8_t u32_source[4] = {0x78u, 0x56u, 0x34u, 0x12u};
    const uint8_t f32_source[4] = {0x3fu, 0xa0u, 0x00u, 0x00u};
    const uint8_t f64_source[8] = {0x00u, 0x00u, 0x00u, 0x00u,
                                   0x00u, 0x00u, 0x04u, 0xc0u};
    uint8_t expected_u8;
    uint32_t expected_u32;
    float expected_f32;
    double expected_f64;
    uint8_t actual_u8;
    uint32_t actual_u32;
    float actual_f32;
    double actual_f64;

    if (load_gold("analysis/time_orbit/getuint8para_gold.bin", &expected_u8, sizeof(expected_u8)) != 0 ||
        load_gold("analysis/time_orbit/getuint32para_gold.bin", &expected_u32, sizeof(expected_u32)) != 0 ||
        load_gold("analysis/time_orbit/getfloat32para_gold.bin", &expected_f32, sizeof(expected_f32)) != 0 ||
        load_gold("analysis/time_orbit/getfloat64para_gold.bin", &expected_f64, sizeof(expected_f64)) != 0) {
        return 1;
    }
    actual_u8 = get_uint8_para(u8_source);
    actual_u32 = get_uint32_para(u32_source, 1u);
    actual_f32 = get_float32_para(f32_source, 0u);
    actual_f64 = get_float64_para(f64_source, 1u);
    if (memcmp(&actual_u8, &expected_u8, sizeof(actual_u8)) != 0 ||
        memcmp(&actual_u32, &expected_u32, sizeof(actual_u32)) != 0 ||
        memcmp(&actual_f32, &expected_f32, sizeof(actual_f32)) != 0 ||
        memcmp(&actual_f64, &expected_f64, sizeof(actual_f64)) != 0) {
        (void)puts("get_*_para original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: get_uint8_para / get_uint32_para / get_float32_para / get_float64_para controlled original-ELF gold compare");
    return 0;
}

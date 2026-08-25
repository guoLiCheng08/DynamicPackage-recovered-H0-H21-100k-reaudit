#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_ipc_telemetry.h"
#include "dynamic_recovered.h"

static int read_blob(const char *path, void *out, size_t bytes)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return 1;
    if (fread(out, 1u, bytes, file) != bytes || fclose(file) != 0) return 1;
    return 0;
}

static uint32_t bits_from_float(float value)
{
    uint32_t bits;
    memcpy(&bits, &value, sizeof(bits));
    return bits;
}

static uint32_t bits_at(const DpIpcSharedFrame *frame, unsigned index)
{
    uint32_t bits;
    memcpy(&bits, frame->raw + DP_IPC_FLOAT_BASE + index * sizeof(float), sizeof(bits));
    return bits;
}

int main(void)
{
    DpMainTelemetryFrame telemetry;
    DpIpcSharedFrame expected;
    unsigned index;

    if (read_blob("analysis/time_orbit/gold_dyn_main_step1_out.bin", &telemetry, sizeof(telemetry)) != 0 ||
        read_blob("analysis/time_orbit/gold_dyn_main_step1_ipc.bin", &expected, sizeof(expected)) != 0) return 2;
    dp_desk_command_state_reset_for_test();
    (void)init_shared();
    sendDynTele(NULL, &telemetry);
    for (index = 3u; index <= 19u; ++index) {
        uint8_t actual;
        if (get_uint8_value((int32_t)index, &actual) != 0 || actual != expected.raw[DP_IPC_U8_BASE + index]) {
            close_shared();
            return 1;
        }
    }
    for (index = 12u; index <= 63u; ++index) {
        float actual;
        if (get_float_value((int32_t)index, &actual) != 0 || bits_from_float(actual) != bits_at(&expected, index)) {
            close_shared();
            return 1;
        }
    }
    for (index = 219u; index <= 220u; ++index) {
        float actual;
        if (get_float_value((int32_t)index, &actual) != 0 || bits_from_float(actual) != bits_at(&expected, index)) {
            close_shared();
            return 1;
        }
    }
    close_shared();
    puts("sendDynTele public ABI original-ELF IPC compare: PASS (bitwise written fields)");
    return 0;
}

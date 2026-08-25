#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_ipc_telemetry.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return 1;
    }
    if (fread(out, 1u, bytes, file) != bytes || fclose(file) != 0) {
        fprintf(stderr, "incomplete gold file %s\n", path);
        return 1;
    }
    return 0;
}

static int compare_step(unsigned step)
{
    DpMainTelemetryFrame telemetry;
    DpIpcSharedFrame actual;
    DpIpcSharedFrame expected;
    char output_name[64];
    char ipc_name[64];
    size_t index;

    (void)snprintf(output_name, sizeof(output_name), "gold_dyn_main_step%u_out.bin", step);
    (void)snprintf(ipc_name, sizeof(ipc_name), "gold_dyn_main_step%u_ipc.bin", step);
    if (read_blob(output_name, &telemetry, sizeof(telemetry)) != 0 ||
        read_blob(ipc_name, &expected, sizeof(expected)) != 0) {
        return 1;
    }
    memset(&actual, 0, sizeof(actual));
    dp_send_dyn_tele(&actual, &telemetry, 0u, 0u, 0u, 0u, 0u);
    for (index = DP_IPC_FLOAT_BASE; index < DP_IPC_SHM_BYTES; ++index) {
        if (actual.raw[index] != expected.raw[index]) {
            fprintf(stderr, "step %u IPC mismatch at +0x%zx: got %02x expected %02x\n", step, index,
                    (unsigned)actual.raw[index], (unsigned)expected.raw[index]);
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    if (compare_step(1u) != 0 || compare_step(2u) != 0) {
        return 1;
    }
    puts("dyn_main -> sendDynTele two-step original-ELF IPC compare: PASS (payload bitwise)");
    return 0;
}

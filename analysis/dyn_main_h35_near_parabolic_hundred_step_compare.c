#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_ipc_telemetry.h"
#include "dynamic_recovered.h"
#include "dynamic_satellite_globals.h"
#include "dynamic_telemetry_layout.h"

#define H35_GOLD_DIR "analysis/time_orbit/"
#define H35_STEP_COUNT 100u
#define H35_STATE_BYTES 0x108u
#define H35_GLOBAL_Y_BYTES (33u * sizeof(double))
#define H35_MAIN_BYTES 0x220u
#define H35_IPC_BYTES (DP_IPC_SHM_BYTES - DP_IPC_FLOAT_BASE)

static int read_gold(const char *name, void *out, size_t bytes)
{
    char path[512];
    FILE *file;
    size_t count;

    (void)snprintf(path, sizeof(path), "%s%s", H35_GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    return fclose(file) != 0 || count != bytes ? -1 : 0;
}

static int compare_blob(const char *label, const void *actual, const void *expected,
                        size_t bytes)
{
    const unsigned char *a = actual;
    const unsigned char *e = expected;
    size_t index;
    unsigned mismatches = 0u;

    for (index = 0u; index < bytes; ++index) {
        if (a[index] != e[index]) {
            if (mismatches < 8u) {
                (void)printf("%s byte+0x%zx actual=%02x expected=%02x\n", label, index,
                             (unsigned)a[index], (unsigned)e[index]);
            }
            ++mismatches;
        }
    }
    (void)printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

static void setup_h35_initial(DpInitialConditions *initial)
{
    *initial = *dp_dynamic_dll_initial_conditions();
    initial->orbit_elements[0] = 700000000.0;
    initial->orbit_elements[1] = 0.99;
    initial->orbit_elements[2] = 0.85;
    initial->orbit_elements[3] = 2.30;
    initial->orbit_elements[4] = 1.10;
    initial->orbit_elements[5] = 0.37;
}

int main(void)
{
    DpInitialConditions initial;
    unsigned char core[0x148] = {0};
    DpMainTelemetryFrame main_output = {{0}};
    DpDeviceControlCommand command = {0};
    DpIpcSharedFrame ipc = {{0}};
    unsigned char *gold_state = NULL;
    unsigned char *gold_y = NULL;
    unsigned char *gold_main = NULL;
    unsigned char *gold_ipc = NULL;
    unsigned step;
    int mismatch = 0;

    gold_state = malloc(H35_STEP_COUNT * H35_STATE_BYTES);
    gold_y = malloc(H35_STEP_COUNT * H35_GLOBAL_Y_BYTES);
    gold_main = malloc(H35_STEP_COUNT * H35_MAIN_BYTES);
    gold_ipc = malloc(H35_STEP_COUNT * H35_IPC_BYTES);
    if (gold_state == NULL || gold_y == NULL || gold_main == NULL || gold_ipc == NULL ||
        read_gold("gold_h35_near_parabolic_hundred_first_state.bin", gold_state,
                  H35_STEP_COUNT * H35_STATE_BYTES) != 0 ||
        read_gold("gold_h35_near_parabolic_hundred_first_global_y.bin", gold_y,
                  H35_STEP_COUNT * H35_GLOBAL_Y_BYTES) != 0 ||
        read_gold("gold_h35_near_parabolic_hundred_first_out.bin", gold_main,
                  H35_STEP_COUNT * H35_MAIN_BYTES) != 0 ||
        read_gold("gold_h35_near_parabolic_hundred_first_ipc_payload.bin", gold_ipc,
                  H35_STEP_COUNT * H35_IPC_BYTES) != 0) {
        free(gold_state);
        free(gold_y);
        free(gold_main);
        free(gold_ipc);
        return 1;
    }

    DynamicDllInit();
    setup_h35_initial(&initial);
    dyn_init(&initial);
    srand(1u);

    for (step = 0u; step < H35_STEP_COUNT; ++step) {
        char label[112];

        dyn_main(&main_output, core, &command);
        (void)snprintf(label, sizeof(label), "H35 near-parabolic[%u] CoreDynamic", step + 1u);
        mismatch |= compare_blob(label, core, gold_state + step * H35_STATE_BYTES,
                                 H35_STATE_BYTES);
        (void)snprintf(label, sizeof(label), "H35 near-parabolic[%u] global y", step + 1u);
        mismatch |= compare_blob(label, y, gold_y + step * H35_GLOBAL_Y_BYTES,
                                 H35_GLOBAL_Y_BYTES);
        (void)snprintf(label, sizeof(label), "H35 near-parabolic[%u] main output", step + 1u);
        mismatch |= compare_blob(label, main_output.raw, gold_main + step * H35_MAIN_BYTES,
                                 H35_MAIN_BYTES);
        memset(&ipc, 0, sizeof(ipc));
        dp_send_dyn_tele(&ipc, &main_output, 0u, 0u, 0u, 0u, 0u);
        (void)snprintf(label, sizeof(label), "H35 near-parabolic[%u] IPC", step + 1u);
        mismatch |= compare_blob(label, ipc.raw + DP_IPC_FLOAT_BASE,
                                 gold_ipc + step * H35_IPC_BYTES, H35_IPC_BYTES);
    }

    free(gold_state);
    free(gold_y);
    free(gold_main);
    free(gold_ipc);
    if (mismatch == 0) {
        (void)puts("PASS: H35 near-parabolic hundred-step bitwise compare");
    }
    return mismatch == 0 ? 0 : 1;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_ipc_telemetry.h"
#include "dynamic_recovered.h"
#include "dynamic_satellite_globals.h"
#include "dynamic_telemetry_layout.h"

#define GOLD_DIR "analysis/time_orbit/"
#define STEP_COUNT 1000u
#define STATE_BYTES 0x108u
#define GLOBAL_Y_BYTES (33u * sizeof(double))
#define MAIN_BYTES 0x220u
#define IPC_BYTES (DP_IPC_SHM_BYTES - DP_IPC_FLOAT_BASE)

static int read_gold(const char *name, void *out, size_t bytes)
{
    char path[512];
    FILE *file;
    size_t count;

    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    return fclose(file) != 0 || count != bytes ? -1 : 0;
}

static int compare_blob(const char *label, const void *actual, const void *expected, size_t bytes)
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

static int seed_startup_ipc(void)
{
    unsigned index;
    int failed = 0;

    failed |= dp_ipc_seed_float_for_test(64u, 1.25f) != 0;
    failed |= dp_ipc_seed_float_for_test(65u, -2.5f) != 0;
    failed |= dp_ipc_seed_float_for_test(66u, 3.75f) != 0;
    for (index = 0u; index < 17u; ++index) {
        failed |= dp_ipc_seed_u8_for_test(20u + index, (unsigned char)(0x80u + index)) != 0;
    }
    return failed;
}

/* H30 原 ELF probe 在同一受控共享帧内先写 startup IPC，再调用 sendDynTele。
 * 这里将这些仍被保留的输入槽位复制到本地 telemetry 帧，避免把比较器的零填充
 * 误判为恢复模型的 IPC 差异。 */
static void seed_startup_ipc_frame(DpIpcSharedFrame *frame)
{
    unsigned index;
    float *floats = (float *)(void *)(frame->raw + DP_IPC_FLOAT_BASE);
    unsigned char *bytes = frame->raw + 0x998u;

    floats[64] = 1.25f;
    floats[65] = -2.5f;
    floats[66] = 3.75f;
    for (index = 0u; index < 17u; ++index) {
        bytes[20u + index] = (unsigned char)(0x80u + index);
    }
    /* 原 ELF 的 getDeskCommand 启动链会在同一 g_data 帧的命令子区保留
     * 四个 DRC 重排槽位；相对 payload 偏移为 0x966/0x967/0x96a/0x96b。 */
    bytes[6] = 0x8bu;
    bytes[7] = 0x8cu;
    bytes[10] = 0x8fu;
    bytes[11] = 0x90u;
}

int main(void)
{
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

    gold_state = malloc(STEP_COUNT * STATE_BYTES);
    gold_y = malloc(STEP_COUNT * GLOBAL_Y_BYTES);
    gold_main = malloc(STEP_COUNT * MAIN_BYTES);
    gold_ipc = malloc(STEP_COUNT * IPC_BYTES);
    if (gold_state == NULL || gold_y == NULL || gold_main == NULL || gold_ipc == NULL ||
        read_gold("gold_startup_chain_h30_thousand_first_state.bin", gold_state, STEP_COUNT * STATE_BYTES) != 0 ||
        read_gold("gold_startup_chain_h30_thousand_first_global_y.bin", gold_y, STEP_COUNT * GLOBAL_Y_BYTES) != 0 ||
        read_gold("gold_startup_chain_h30_thousand_first_out.bin", gold_main, STEP_COUNT * MAIN_BYTES) != 0 ||
        read_gold("gold_startup_chain_h30_thousand_first_ipc_payload.bin", gold_ipc, STEP_COUNT * IPC_BYTES) != 0) {
        free(gold_state); free(gold_y); free(gold_main); free(gold_ipc);
        return 1;
    }

    DynamicDllInit();
    dp_desk_command_state_reset_for_test();
    if (seed_startup_ipc() != 0) {
        (void)fputs("cannot seed H30 controlled IPC startup input\n", stderr);
        free(gold_state); free(gold_y); free(gold_main); free(gold_ipc);
        return 1;
    }
    getDeskCommand();
    dyn_init(dp_dynamic_dll_initial_conditions());

    for (step = 0u; step < STEP_COUNT; ++step) {
        char label[112];

        dyn_main(&main_output, core, &command);
        (void)snprintf(label, sizeof(label), "H30 startup chain[%u] core", step + 1u);
        mismatch |= compare_blob(label, core, gold_state + step * STATE_BYTES, STATE_BYTES);
        (void)snprintf(label, sizeof(label), "H30 startup chain[%u] global y", step + 1u);
        mismatch |= compare_blob(label, y, gold_y + step * GLOBAL_Y_BYTES, GLOBAL_Y_BYTES);
        (void)snprintf(label, sizeof(label), "H30 startup chain[%u] main", step + 1u);
        mismatch |= compare_blob(label, main_output.raw, gold_main + step * MAIN_BYTES, MAIN_BYTES);
        dp_send_dyn_tele(&ipc, &main_output, 0u, 0u, 0u, 0u, 0u);
        seed_startup_ipc_frame(&ipc);
        (void)snprintf(label, sizeof(label), "H30 startup chain[%u] IPC", step + 1u);
        mismatch |= compare_blob(label, ipc.raw + DP_IPC_FLOAT_BASE, gold_ipc + step * IPC_BYTES, IPC_BYTES);
    }
    close_shared();
    free(gold_state); free(gold_y); free(gold_main); free(gold_ipc);
    if (mismatch == 0) {
        (void)puts("PASS: H30 startup chain thousand-step bitwise compare");
        return 0;
    }
    return 1;
}

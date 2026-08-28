#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "dynamic_ipc_telemetry.h"

#define H33_GOLD_DIR "analysis/coverage_inventory/p0_command_family/"
#define H33_RETURNS_BYTES (8u * sizeof(int32_t))

static int read_gold(const char *name, void *out, size_t bytes)
{
    char path[512];
    FILE *file;
    size_t count;

    (void)snprintf(path, sizeof(path), "%s%s", H33_GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return -1;
    }
    count = fread(out, 1u, bytes, file);
    if (fclose(file) != 0 || count != bytes) {
        return -1;
    }
    return 0;
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
                printf("%s byte+0x%zx actual=%02x expected=%02x\n", label, index,
                       (unsigned)a[index], (unsigned)e[index]);
            }
            ++mismatches;
        }
    }
    printf("%s mismatched bytes: %u/%zu\n", label, mismatches, bytes);
    return mismatches == 0u ? 0 : -1;
}

int main(void)
{
    int32_t expected_returns[8];
    int32_t actual_returns[8];
    float expected_invalid_float;
    float expected_reopen_float;
    float actual_invalid_float = 123.5f;
    float actual_reopen_float = 77.25f;
    uint8_t expected_u8;
    uint8_t actual_u8 = 0x5au;
    int mismatch = 0;

    if (read_gold("gold_ipc_getter_h33_invalid_first_returns.bin", expected_returns,
                  sizeof(expected_returns)) ||
        read_gold("gold_ipc_getter_h33_invalid_first_invalid_float_out.bin",
                  &expected_invalid_float, sizeof(expected_invalid_float)) ||
        read_gold("gold_ipc_getter_h33_invalid_first_reopen_float_out.bin",
                  &expected_reopen_float, sizeof(expected_reopen_float)) ||
        read_gold("gold_ipc_getter_h33_invalid_first_u8_out.bin", &expected_u8,
                  sizeof(expected_u8))) {
        return 1;
    }

    (void)shm_unlink(DP_IPC_SHM_NAME);
    actual_returns[0] = init_shared();
    actual_returns[1] = get_float_value(-1, &actual_invalid_float);
    actual_returns[2] = get_float_value(0x258, &actual_invalid_float);
    actual_returns[3] = get_float_value(0, NULL);
    actual_returns[4] = get_uint8_value(-1, &actual_u8);
    actual_returns[5] = get_uint8_value(0x258, &actual_u8);
    actual_returns[6] = get_uint8_value(0, NULL);
    close_shared();
    actual_returns[7] = get_float_value(0, &actual_reopen_float);

    mismatch |= compare_blob("H33 getter return codes", actual_returns, expected_returns,
                             H33_RETURNS_BYTES);
    mismatch |= compare_blob("H33 invalid float sentinel", &actual_invalid_float,
                             &expected_invalid_float, sizeof(actual_invalid_float));
    mismatch |= compare_blob("H33 reopen float output", &actual_reopen_float,
                             &expected_reopen_float, sizeof(actual_reopen_float));
    mismatch |= compare_blob("H33 getter uint8 sentinel", &actual_u8, &expected_u8,
                             sizeof(actual_u8));

    close_shared();
    (void)shm_unlink(DP_IPC_SHM_NAME);
    if (mismatch == 0) {
        printf("PASS: H33 shared IPC invalid-index and reopen bitwise compare\n");
    }
    return mismatch == 0 ? 0 : 1;
}

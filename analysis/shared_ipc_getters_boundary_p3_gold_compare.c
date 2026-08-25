#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "dynamic_ipc_telemetry.h"

#define RETURNS_GOLD "analysis/time_orbit/gold_shared_ipc_getters_boundary_p3_returns.bin"
#define FLOATS_GOLD "analysis/time_orbit/gold_shared_ipc_getters_boundary_p3_floats.bin"
#define U8_GOLD "analysis/time_orbit/gold_shared_ipc_getters_boundary_p3_u8.bin"

static int read_gold(const char *path, void *out, size_t bytes)
{
    FILE *file = fopen(path, "rb");
    size_t count;
    if (file == NULL) {
        perror(path);
        return -1;
    }
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
    int fd = -1;
    DpIpcMappedShared *writer = MAP_FAILED;
    int32_t actual_returns[8];
    float actual_floats[2] = {0.0f, 0.0f};
    uint8_t actual_u8 = 0u;
    unsigned char expected_returns[sizeof(actual_returns)];
    unsigned char expected_floats[sizeof(actual_floats)];
    unsigned char expected_u8[sizeof(actual_u8)];
    const uint32_t last_float_bits = 0xff800000u;
    int mismatch = 0;

    (void)shm_unlink(DP_IPC_SHM_NAME);
    close_shared();
    actual_returns[0] = init_shared();
    if (actual_returns[0] != 0) {
        fprintf(stderr, "init_shared failed: %d\n", actual_returns[0]);
        return 1;
    }
    fd = shm_open(DP_IPC_SHM_NAME, O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open writer");
        close_shared();
        return 1;
    }
    writer = mmap(NULL, DP_IPC_SHM_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (writer == MAP_FAILED) {
        perror("mmap writer");
        (void)close(fd);
        close_shared();
        return 1;
    }
    (void)pthread_rwlock_wrlock(&writer->lock);
    memcpy(((uint8_t *)writer) + DP_IPC_FLOAT_BASE + 599u * sizeof(float),
           &last_float_bits, sizeof(last_float_bits));
    ((uint8_t *)writer)[DP_IPC_U8_BASE + 599u] = 0x5au;
    (void)pthread_rwlock_unlock(&writer->lock);
    actual_returns[1] = get_float_value(599, &actual_floats[0]);
    actual_returns[2] = get_uint8_value(599, &actual_u8);
    actual_returns[3] = get_float_value(-1, &actual_floats[0]);
    actual_returns[4] = get_uint8_value(-1, &actual_u8);
    actual_returns[5] = get_float_value(0, NULL);
    actual_returns[6] = get_uint8_value(0, NULL);
    close_shared();
    actual_returns[7] = get_float_value(599, &actual_floats[1]);
    if (read_gold(RETURNS_GOLD, expected_returns, sizeof(expected_returns)) != 0 ||
        read_gold(FLOATS_GOLD, expected_floats, sizeof(expected_floats)) != 0 ||
        read_gold(U8_GOLD, expected_u8, sizeof(expected_u8)) != 0) {
        (void)munmap(writer, DP_IPC_SHM_BYTES);
        (void)close(fd);
        close_shared();
        (void)shm_unlink(DP_IPC_SHM_NAME);
        return 1;
    }
    mismatch |= compare("shared IPC getter boundary P3 returns", actual_returns,
                        expected_returns, sizeof(actual_returns));
    mismatch |= compare("shared IPC getter boundary P3 floats", actual_floats,
                        expected_floats, sizeof(actual_floats));
    mismatch |= compare("shared IPC getter boundary P3 uint8", &actual_u8,
                        expected_u8, sizeof(actual_u8));
    (void)munmap(writer, DP_IPC_SHM_BYTES);
    (void)close(fd);
    close_shared();
    (void)shm_unlink(DP_IPC_SHM_NAME);
    if (mismatch == 0) puts("shared IPC getter boundary P3 original-ELF compare: PASS (bitwise)");
    return mismatch == 0 ? 0 : 1;
}

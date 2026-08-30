#define _POSIX_C_SOURCE 200809L

#include "dynamic_ipc_telemetry.h"
#include "c_shadow_state_ipc.h"
#include "shadow_replay_ipc.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

static int publish_elf_input(const char *path, const float values[16])
{
    int fd = open(path, O_RDWR | O_CLOEXEC);
    DpIpcMappedShared *mapped;
    unsigned index;

    if (fd < 0) return -1;
    mapped = mmap(NULL, sizeof(*mapped), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    (void)close(fd);
    if (mapped == MAP_FAILED) return -1;
    if (pthread_rwlock_wrlock(&mapped->lock) != 0) {
        (void)munmap(mapped, sizeof(*mapped));
        return -1;
    }
    for (index = 0u; index < 16u; ++index) {
        memcpy((unsigned char *)mapped + DP_IPC_FLOAT_BASE + index * sizeof(float),
               &values[index], sizeof(values[index]));
    }
    (void)pthread_rwlock_unlock(&mapped->lock);
    (void)munmap(mapped, sizeof(*mapped));
    return 0;
}

static int wait_sequence(const DpCShadowStateFrame *frame, uint32_t expected)
{
    unsigned attempts;

    for (attempts = 0u; attempts < 30000u; ++attempts) {
        if (__atomic_load_n(&frame->sequence, __ATOMIC_ACQUIRE) >= expected) return 0;
        {
            const struct timespec delay = {0, 1000000};
            nanosleep(&delay, NULL);
        }
    }
    return -1;
}

static DpCShadowStateFrame *open_state(const char *name, int *fd)
{
    DpCShadowStateFrame *frame;

    *fd = shm_open(name, O_RDONLY, 0);
    if (*fd < 0) return NULL;
    frame = mmap(NULL, sizeof(*frame), PROT_READ, MAP_SHARED, *fd, 0);
    (void)close(*fd);
    return frame == MAP_FAILED ? NULL : frame;
}

int main(int argc, char **argv)
{
    const char *elf_input_path = getenv("ELF_INPUT_FD");
    const char *elf_state_name = getenv("ELF_C_SHADOW_STATE_SHM");
    DpShadowReplayFrame *replay;
    DpCShadowStateFrame *c_state;
    DpCShadowStateFrame *elf_state;
    float values[16] = {0.0f};
    unsigned long count;
    unsigned long sequence;
    int replay_fd;
    int c_fd;
    int elf_fd;

    if (argc != 2 || elf_input_path == NULL || elf_input_path[0] == '\0') return 2;
    if (elf_state_name == NULL || elf_state_name[0] != '/') elf_state_name = "/cfs_test_elf_state";
    count = strtoul(argv[1], NULL, 10);
    if (count == 0ul || count > UINT32_MAX) return 2;
    replay_fd = shm_open(DP_SHADOW_INPUT_SHM, O_RDWR, 0);
    if (replay_fd < 0) return 3;
    replay = mmap(NULL, sizeof(*replay), PROT_READ | PROT_WRITE, MAP_SHARED, replay_fd, 0);
    (void)close(replay_fd);
    c_state = open_state(DP_C_SHADOW_STATE_SHM, &c_fd);
    elf_state = open_state(elf_state_name, &elf_fd);
    if (replay == MAP_FAILED || c_state == NULL || elf_state == NULL) return 3;

    for (sequence = 1ul; sequence <= count; ++sequence) {
        if (publish_elf_input(elf_input_path, values) != 0) return 4;
        replay->step_time = 0.01;
        memset(&replay->command, 0, sizeof(replay->command));
        __atomic_store_n(&replay->sequence, (uint32_t)sequence, __ATOMIC_RELEASE);
        if (wait_sequence(c_state, (uint32_t)sequence) != 0 ||
            wait_sequence(elf_state, (uint32_t)sequence) != 0) return 5;
    }
    printf("1. 结果=通过\n2. 双路闭环输入帧=%lu\n3. 积分步长=0.01秒\n4. 仿真时长=%.2f秒\n",
           count, (double)count * 0.01);
    return 0;
}

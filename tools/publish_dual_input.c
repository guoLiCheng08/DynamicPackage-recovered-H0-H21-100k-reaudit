#define _POSIX_C_SOURCE 200809L

#include "dynamic_ipc_telemetry.h"
#include "shadow_replay_ipc.h"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static int parse_double(const char *text, double *value)
{
    char *end;
    *value = strtod(text, &end);
    return end != text && *end == '\0';
}

static int publish_elf_input(const char *elf_input_path, const float values[12],
                             const uint8_t flags[3])
{
    int fd = open(elf_input_path, O_RDWR | O_CLOEXEC);
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
    for (index = 0u; index < 12u; ++index) {
        memcpy((unsigned char *)mapped + DP_IPC_FLOAT_BASE + index * sizeof(float),
               &values[index], sizeof(values[index]));
    }
    memcpy((unsigned char *)mapped + DP_IPC_U8_BASE, flags, 3u);
    (void)pthread_rwlock_unlock(&mapped->lock);
    (void)munmap(mapped, sizeof(*mapped));
    return 0;
}

int main(int argc, char **argv)
{
    DpShadowReplayFrame *replay;
    float elf_values[12] = {0.0f};
    uint8_t elf_flags[3];
    double values[15];
    int replay_fd;
    unsigned index;
    uint32_t sequence;

    /* sequence + 4 wheel + 6 MTQ + SADA flag + 2 SADA angle +
     * thruster status + inertia flag. */
    if (argc != 17) {
        fprintf(stderr, "用法: %s 序号 轮x4 磁力矩x6 太敏标志 太敏角x2 推进器状态 惯量标志\n", argv[0]);
        return 2;
    }
    sequence = (uint32_t)strtoul(argv[1], NULL, 10);
    if (sequence == 0u) return 2;
    for (index = 0u; index < 15u; ++index) {
        if (!parse_double(argv[index + 2], &values[index])) return 2;
    }
    for (index = 0u; index < 4u; ++index) elf_values[index] = (float)values[index];
    for (index = 0u; index < 6u; ++index) elf_values[index + 4u] = (float)values[index + 4u];
    elf_values[10] = (float)values[11];
    elf_values[11] = (float)values[12];
    elf_flags[0] = values[0] != 0.0 || values[1] != 0.0 ||
                   values[2] != 0.0 || values[3] != 0.0;
    elf_flags[1] = (uint8_t)values[10];
    elf_flags[2] = values[13] != 0.0;
    {
        const char *elf_input_path = getenv("ELF_INPUT_FD");
        if (elf_input_path == NULL || elf_input_path[0] == '\0')
            elf_input_path = DP_IPC_SHM_NAME;
        if (publish_elf_input(elf_input_path, elf_values, elf_flags) != 0) {
            fprintf(stderr, "无法写入正式 ELF 输入共享区 %s: %s\n",
                    elf_input_path, strerror(errno));
            return 1;
        }
    }

    replay_fd = shm_open(DP_SHADOW_INPUT_SHM, O_CREAT | O_RDWR, 0600);
    if (replay_fd < 0 || ftruncate(replay_fd, (off_t)sizeof(*replay)) != 0) return 1;
    replay = mmap(NULL, sizeof(*replay), PROT_READ | PROT_WRITE, MAP_SHARED,
                  replay_fd, 0);
    if (replay == MAP_FAILED) {
        (void)close(replay_fd);
        return 1;
    }
    replay->magic = DP_SHADOW_INPUT_MAGIC;
    replay->version = DP_SHADOW_INPUT_VERSION;
    replay->reserved = 0u;
    replay->step_time = 0.01;
    memset(&replay->command, 0, sizeof(replay->command));
    for (index = 0u; index < 4u; ++index) replay->command.wheel_torque_command[index] = values[index];
    for (index = 0u; index < 6u; ++index) replay->command.mtq_moment_command[index] = values[index + 4u];
    replay->command.sada_command_flag = elf_flags[1];
    replay->command.sada_command_angle[0] = (double)elf_values[10];
    replay->command.sada_command_angle[1] = (double)elf_values[11];
    replay->command.thruster_work_status = elf_flags[2];
    replay->command.inertia_update_flag = 0u;
    (void)msync(replay, sizeof(*replay), MS_SYNC);
    __atomic_store_n(&replay->sequence, sequence, __ATOMIC_RELEASE);
    (void)msync((unsigned char *)replay + offsetof(DpShadowReplayFrame, sequence),
                sizeof(replay->sequence), MS_SYNC);
    (void)munmap(replay, sizeof(*replay));
    (void)close(replay_fd);
    printf("1. 结果=通过\n2. 输入序号=%u\n3. ELF输入=%s\n4. 纯C输入=%s\n",
           sequence, getenv("ELF_INPUT_FD") != NULL ? getenv("ELF_INPUT_FD") : DP_IPC_SHM_NAME,
           DP_SHADOW_INPUT_SHM);
    return 0;
}

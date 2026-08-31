#define _POSIX_C_SOURCE 200809L

#include "dynamic_ipc_telemetry.h"
#include "c_shadow_state_ipc.h"
#include "shadow_replay_ipc.h"

#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

static int publish_elf_input(const char *path, const float values[12],
                             const uint8_t flags[3])
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
    for (index = 0u; index < 12u; ++index) {
        memcpy((unsigned char *)mapped + DP_IPC_FLOAT_BASE + index * sizeof(float),
               &values[index], sizeof(values[index]));
    }
    memcpy((unsigned char *)mapped + DP_IPC_U8_BASE, flags, 3u);
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

static int parse_command_line(char *line, double command_values[15])
{
    char *cursor = line;
    char *end;
    unsigned index;

    while (isspace((unsigned char)*cursor)) ++cursor;
    if (*cursor == '\0' || *cursor == '#') return 0;
    for (index = 0u; index < 15u; ++index) {
        command_values[index] = strtod(cursor, &end);
        if (end == cursor) return -1;
        cursor = end;
        while (*cursor == ',' || *cursor == ';' || isspace((unsigned char)*cursor)) ++cursor;
    }
    return *cursor == '\0' || *cursor == '#' ? 1 : -1;
}

static int publish_command(DpShadowReplayFrame *replay, uint32_t sequence,
                           const double command_values[15])
{
    float elf_values[12] = {0.0f};
    uint8_t elf_flags[3];
    unsigned index;

    for (index = 0u; index < 4u; ++index) elf_values[index] = (float)command_values[index];
    for (index = 0u; index < 6u; ++index) elf_values[index + 4u] = (float)command_values[index + 4u];
    elf_values[10] = (float)command_values[11];
    elf_values[11] = (float)command_values[12];
    elf_flags[0] = command_values[0] != 0.0 || command_values[1] != 0.0 ||
                   command_values[2] != 0.0 || command_values[3] != 0.0;
    elf_flags[1] = (uint8_t)command_values[10];
    elf_flags[2] = command_values[13] != 0.0;
    if (publish_elf_input(getenv("ELF_INPUT_FD"), elf_values, elf_flags) != 0) return -1;

    replay->step_time = 0.01;
    memset(&replay->command, 0, sizeof(replay->command));
    for (index = 0u; index < 4u; ++index)
        replay->command.wheel_torque_command[index] = (double)elf_values[index];
    for (index = 0u; index < 6u; ++index)
        replay->command.mtq_moment_command[index] = (double)elf_values[index + 4u];
    replay->command.sada_command_flag = elf_flags[1];
    replay->command.sada_command_angle[0] = (double)elf_values[10];
    replay->command.sada_command_angle[1] = (double)elf_values[11];
    replay->command.thruster_work_status = elf_flags[2];
    /* 当前真实共享输入协议尚未确认惯量更新字段，不能把内部测试字段伪装成
     * ELF 已消费的外部输入。 */
    replay->command.inertia_update_flag = 0u;
    __atomic_store_n(&replay->sequence, sequence, __ATOMIC_RELEASE);
    return 0;
}

static int command_matches(const DpDeviceControlCommand *requested,
                           const DpDeviceControlCommand *applied)
{
    return memcmp(requested->wheel_torque_command, applied->wheel_torque_command,
                  sizeof(requested->wheel_torque_command)) == 0 &&
           memcmp(requested->mtq_moment_command, applied->mtq_moment_command,
                  sizeof(requested->mtq_moment_command)) == 0 &&
           requested->sada_command_flag == applied->sada_command_flag &&
           memcmp(requested->sada_command_angle, applied->sada_command_angle,
                  sizeof(requested->sada_command_angle)) == 0 &&
           requested->thruster_work_status == applied->thruster_work_status &&
           requested->inertia_update_flag == applied->inertia_update_flag;
}

int main(int argc, char **argv)
{
    const char *elf_input_path = getenv("ELF_INPUT_FD");
    const char *elf_state_name = getenv("ELF_C_SHADOW_STATE_SHM");
    DpShadowReplayFrame *replay;
    DpCShadowStateFrame *c_state;
    DpCShadowStateFrame *elf_state;
    double command_values[15] = {0.0};
    FILE *input = NULL;
    char line[1024];
    unsigned long count;
    unsigned long sequence;
    int replay_fd;
    int c_fd;
    int elf_fd;
    const char *delivery_log_path = getenv("INPUT_DELIVERY_LOG");
    FILE *delivery_log = NULL;
    unsigned long delivery_mismatch_count = 0ul;
    unsigned long formal_count = 0ul;
    const char *primer_text = getenv("C_SHADOW_STARTUP_PRIMER");
    const unsigned long primer_count = primer_text != NULL ? strtoul(primer_text, NULL, 10) : 0ul;

    if (argc == 3 && strcmp(argv[1], "--csv") == 0) {
        input = fopen(argv[2], "r");
        if (input == NULL) return 2;
    } else if (argc != 2) {
        return 2;
    }
    if (elf_input_path == NULL || elf_input_path[0] == '\0') return 2;
    if (elf_state_name == NULL || elf_state_name[0] != '/') elf_state_name = "/cfs_test_elf_state";
    count = input == NULL ? strtoul(argv[1], NULL, 10) : 0ul;
    if ((input == NULL && count == 0ul) || count > UINT32_MAX) return 2;
    replay_fd = shm_open(dp_shadow_input_shm_name(), O_RDWR, 0);
    if (replay_fd < 0) return 3;
    replay = mmap(NULL, sizeof(*replay), PROT_READ | PROT_WRITE, MAP_SHARED, replay_fd, 0);
    (void)close(replay_fd);
    c_state = open_state(dp_c_shadow_state_shm_name(), &c_fd);
    elf_state = open_state(elf_state_name, &elf_fd);
    if (replay == MAP_FAILED || c_state == NULL || elf_state == NULL) return 3;
    if (delivery_log_path != NULL && delivery_log_path[0] != '\0') {
        delivery_log = fopen(delivery_log_path, "w");
        if (delivery_log == NULL) return 3;
    }

    if (primer_count != 0ul) {
        const double primer_values[15] = {0.0};
        unsigned long primer_sequence;

        for (primer_sequence = 1ul; primer_sequence <= primer_count; ++primer_sequence) {
            if (publish_command(replay, (uint32_t)primer_sequence, primer_values) != 0 ||
                wait_sequence(c_state, (uint32_t)primer_sequence) != 0 ||
                wait_sequence(elf_state, (uint32_t)primer_sequence) != 0)
                return 5;
            if (delivery_log != NULL) {
                const int matched = command_matches(&replay->command,
                                                    &elf_state->applied_command);
                (void)fprintf(delivery_log, "预置帧%lu 请求与ELF实际输入=%s（不计入正式统计）\n",
                              primer_sequence, matched ? "一致" : "不一致");
                (void)fflush(delivery_log);
            }
        }
    }

    for (sequence = primer_count + 1ul;
         input == NULL ? formal_count < count : fgets(line, sizeof(line), input) != NULL;
         ++sequence) {
        if (input != NULL) {
            int parsed = parse_command_line(line, command_values);
            if (parsed == 0) {
                --sequence;
                continue;
            }
            if (parsed < 0) return 2;
        }
        if (publish_command(replay, (uint32_t)sequence, command_values) != 0) return 4;
        if (wait_sequence(c_state, (uint32_t)sequence) != 0 ||
            wait_sequence(elf_state, (uint32_t)sequence) != 0) return 5;
        if (delivery_log != NULL) {
            const int matched = command_matches(&replay->command,
                                                &elf_state->applied_command);
            if (!matched) ++delivery_mismatch_count;
            (void)fprintf(delivery_log, "%lu 请求与ELF实际输入=%s\n", sequence,
                          matched ? "一致" : "不一致");
            (void)fflush(delivery_log);
        }
        ++formal_count;
    }
    if (input != NULL) {
        (void)fclose(input);
        count = formal_count;
    }
    printf("1. 结果=通过\n2. 双路闭环输入帧=%lu\n3. 积分步长=0.01秒\n4. 仿真时长=%.2f秒\n",
           count, (double)count * 0.01);
    if (delivery_log != NULL) {
        (void)fprintf(delivery_log, "汇总 正式请求与ELF实际输入不一致=%lu\n",
                      delivery_mismatch_count);
        (void)fclose(delivery_log);
        printf("5. 输入交付不一致=%lu\n", delivery_mismatch_count);
    }
    return 0;
}

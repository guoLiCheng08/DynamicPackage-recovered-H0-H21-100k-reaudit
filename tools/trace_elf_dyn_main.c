#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include "c_shadow_state_ipc.h"
#include "shadow_replay_ipc.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define ELF_DYN_MAIN_OFFSET 0x4020u
#define ELF_Y_OFFSET 0x2184e0u
#define ELF_OUTPUT_OFFSET 0x218760u
#define ELF_STATE_SHM "/cfs_test_elf_state"
#define ELF_T_OFFSET 0x2184c0u
#define ELF_TIME_SECOND_DECIMAL_OFFSET 0x218680u
#define ELF_TIME_SECOND_TOTAL_OFFSET 0x218688u
#define ELF_TIME_CALENDAR_OFFSET 0x2186a0u
#define ELF_TIME_TM_OFFSET 0x218be0u

static int read_process_bytes(pid_t pid, unsigned long address, void *buffer, size_t bytes);

static int read_device_snapshot(pid_t pid, unsigned long base,
                                uint8_t snapshot[DP_DEVICE_GLOBAL_SNAPSHOT_BYTES])
{
    struct {
        unsigned long offset;
        unsigned long bytes;
    } regions[] = {
        {0x217540u, 0x450u}, {0x216a00u, 0x640u}, {0x2179a0u, 0x2b0u},
        {0x2172c0u, 0x270u}, {0x216900u, 0x70u}, {0x216980u, 0x68u},
        {0x217e00u, 0x1e0u}, {0x217160u, 0x150u}, {0x217c60u, 0xb0u},
        {0x217d20u, 0xc8u}, {0x217040u, 0x110u}
    };
    unsigned index;
    size_t destination = 0u;
    memset(snapshot, 0, DP_DEVICE_GLOBAL_SNAPSHOT_BYTES);
    for (index = 0u; index < sizeof(regions) / sizeof(regions[0]); ++index) {
        if (read_process_bytes(pid, base + regions[index].offset,
                                snapshot + destination, regions[index].bytes) != 0)
            return -1;
        destination += regions[index].bytes;
    }
    return destination == DP_DEVICE_GLOBAL_SNAPSHOT_BYTES ? 0 : -1;
}

static int read_process_bytes(pid_t pid, unsigned long address, void *buffer, size_t bytes)
{
    unsigned char *destination = buffer;
    size_t offset;
    for (offset = 0u; offset < bytes; offset += sizeof(unsigned long)) {
        unsigned long word;
        size_t remaining = bytes - offset;
        size_t copy_bytes = remaining < sizeof(word) ? remaining : sizeof(word);
        errno = 0;
        word = (unsigned long)ptrace(PTRACE_PEEKDATA, pid,
                                     (void *)(address + offset), NULL);
        if (errno != 0) return -1;
        memcpy(destination + offset, &word, copy_bytes);
    }
    return 0;
}

static int process_base(pid_t pid, unsigned long *base)
{
    char path[64];
    char line[512];
    FILE *maps;
    (void)snprintf(path, sizeof(path), "/proc/%ld/maps", (long)pid);
    maps = fopen(path, "r");
    if (maps == NULL) return -1;
    while (fgets(line, sizeof(line), maps) != NULL) {
        unsigned long start, end, offset;
        char permissions[5];
        char mapped_path[256] = {0};
        if (sscanf(line, "%lx-%lx %4s %lx %*s %*s %255s", &start, &end,
                   permissions, &offset, mapped_path) >= 5 &&
            strcmp(permissions, "r-xp") == 0 && offset == 0u &&
            strstr(mapped_path, "DynamicPackage") != NULL) {
            *base = start;
            fclose(maps);
            return 0;
        }
    }
    fclose(maps);
    return -1;
}

static int patch_byte(pid_t pid, unsigned long address, unsigned long *saved)
{
    unsigned long word;
    errno = 0;
    word = (unsigned long)ptrace(PTRACE_PEEKTEXT, pid, (void *)address, NULL);
    if (errno != 0) return -1;
    if (saved != NULL) *saved = word;
    word = (word & ~0xfful) | 0xccul;
    return ptrace(PTRACE_POKETEXT, pid, (void *)address, (void *)word) == 0 ? 0 : -1;
}

static int restore_word(pid_t pid, unsigned long address, unsigned long word)
{
    return ptrace(PTRACE_POKETEXT, pid, (void *)address, (void *)word) == 0 ? 0 : -1;
}

static int wait_for_input_sequence(const DpShadowReplayFrame *frame, uint32_t expected)
{
    unsigned attempts = 0u;
    while (frame->sequence != expected && attempts < 30000u) {
        struct timespec delay = {0, 1000000};
        nanosleep(&delay, NULL);
        ++attempts;
    }
    return frame->sequence == expected ? 0 : -1;
}

static int wait_for_post_ack(const DpCShadowStateFrame *frame, uint32_t expected)
{
    unsigned attempts = 0u;
    while (__atomic_load_n(&frame->post_ack, __ATOMIC_ACQUIRE) < expected &&
           attempts < 30000u) {
        const struct timespec delay = {0, 1000000};
        nanosleep(&delay, NULL);
        ++attempts;
    }
    return __atomic_load_n(&frame->post_ack, __ATOMIC_ACQUIRE) >= expected ? 0 : -1;
}

static int touch_ready_file(const char *path)
{
    FILE *ready;
    if (path == NULL || path[0] == '\0') return 0;
    ready = fopen(path, "w");
    if (ready == NULL) return -1;
    (void)fputs("ready\n", ready);
    (void)fclose(ready);
    return 0;
}

static int read_process_double(pid_t pid, unsigned long address, double *value)
{
    unsigned long bits;
    errno = 0;
    bits = (unsigned long)ptrace(PTRACE_PEEKDATA, pid, (void *)address, NULL);
    if (errno != 0) return -1;
    memcpy(value, &bits, sizeof(*value));
    return 0;
}

static void print_seed_state(const double state[DP_STATE_DIM])
{
    unsigned index;

    if (getenv("ELF_TRACE_DIAGNOSTIC") == NULL) {
        return;
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        printf("seed_y[%u]=%.17g\n", index, state[index]);
    }
}

static uint32_t rng_record_count(const char *path)
{
    struct stat info;
    if (path == NULL || path[0] == '\0' || stat(path, &info) != 0)
        return 0u;
    return (uint32_t)((uint64_t)info.st_size / sizeof(int32_t));
}

static int read_elf_ipc_payload(pid_t pid, uint8_t payload[DP_IPC_PAYLOAD_BYTES])
{
    char path[64];
    int fd;
    DpIpcMappedShared *mapped;
    (void)snprintf(path, sizeof(path), "/proc/%ld/fd/3", (long)pid);
    fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    mapped = mmap(NULL, sizeof(*mapped), PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (mapped == MAP_FAILED) return -1;
    memcpy(payload, mapped->payload, DP_IPC_PAYLOAD_BYTES);
    (void)munmap(mapped, sizeof(*mapped));
    return 0;
}

static int clear_elf_ipc_payload(pid_t pid)
{
    char path[64];
    int fd;
    DpIpcMappedShared *mapped;
    (void)snprintf(path, sizeof(path), "/proc/%ld/fd/3", (long)pid);
    fd = open(path, O_RDWR);
    if (fd < 0) return -1;
    mapped = mmap(NULL, sizeof(*mapped), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (mapped == MAP_FAILED) return -1;
    if (pthread_rwlock_wrlock(&mapped->lock) != 0) {
        (void)munmap(mapped, sizeof(*mapped));
        return -1;
    }
    memset(mapped->payload, 0, DP_IPC_PAYLOAD_BYTES);
    (void)pthread_rwlock_unlock(&mapped->lock);
    (void)munmap(mapped, sizeof(*mapped));
    return 0;
}

int main(int argc, char **argv)
{
    pid_t pid;
    unsigned long base;
    unsigned long entry_saved = 0ul;
    unsigned long return_address = 0ul;
    unsigned long return_saved = 0ul;
    unsigned samples = argc >= 3 ? (unsigned)strtoul(argv[2], NULL, 10) : 1u;
    uint32_t expected_input_sequence = argc >= 4
        ? (uint32_t)strtoul(argv[3], NULL, 10) : 0u;
    uint32_t active_input_sequence = 0u;
    unsigned captured = 0u;
    int status;
    int entry_breakpoint = 0;
    int return_breakpoint = 0;
    const char *ready_path = getenv("ELF_TRACE_READY_FILE");
    const char *rng_arm_path = getenv("ELF_RNG_ARM_FILE");
    const char *state_log_path = getenv("ELF_STATE_LOG");
    const char *state_shm_name = getenv("ELF_C_SHADOW_STATE_SHM");
    FILE *state_log = NULL;
    int state_fd;
    DpCShadowStateFrame *state_frame;
    DpShadowReplayFrame *input_frame;
    int input_fd;

    if (argc < 2 || samples == 0u) {
        fprintf(stderr, "用法: %s ELF进程号 [采样次数]\n", argv[0]);
        return 2;
    }
    if (state_shm_name == NULL || state_shm_name[0] != '/') {
        state_shm_name = ELF_STATE_SHM;
    }
    pid = (pid_t)strtol(argv[1], NULL, 10);
    if (process_base(pid, &base) != 0 || ptrace(PTRACE_ATTACH, pid, NULL, NULL) != 0 ||
        waitpid(pid, &status, 0) < 0 || !WIFSTOPPED(status)) {
        fprintf(stderr, "无法附加正式 ELF: %s\n", strerror(errno));
        return 3;
    }
    state_fd = shm_open(state_shm_name, O_CREAT | O_RDWR, 0600);
    input_fd = shm_open(DP_SHADOW_INPUT_SHM, O_RDONLY, 0);
    if (state_fd < 0 || input_fd < 0 ||
        ftruncate(state_fd, (off_t)sizeof(*state_frame)) != 0) {
        fprintf(stderr, "无法建立 ELF 状态/输入映射\n");
        (void)ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return 3;
    }
    state_frame = mmap(NULL, sizeof(*state_frame), PROT_READ | PROT_WRITE,
                       MAP_SHARED, state_fd, 0);
    input_frame = mmap(NULL, sizeof(*input_frame), PROT_READ, MAP_SHARED, input_fd, 0);
    close(state_fd);
    close(input_fd);
    if (state_frame == MAP_FAILED || input_frame == MAP_FAILED ||
        patch_byte(pid, base + ELF_DYN_MAIN_OFFSET, &entry_saved) != 0) {
        fprintf(stderr, "无法设置 dyn_main 入口断点\n");
        (void)ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return 3;
    }
    entry_breakpoint = 1;
    state_frame->magic = DP_C_SHADOW_STATE_MAGIC;
    state_frame->version = DP_C_SHADOW_STATE_VERSION;
    state_frame->state_bytes = sizeof(state_frame->state);
    state_frame->devices_bytes = 0u;
    state_frame->seed_ready = 0u;
    state_frame->seed_ack = 0u;
    state_frame->post_ack = 0u;
    memset(state_frame, 0, sizeof(*state_frame));
    state_frame->magic = DP_C_SHADOW_STATE_MAGIC;
    state_frame->version = DP_C_SHADOW_STATE_VERSION;
    state_frame->state_bytes = sizeof(state_frame->state);
    state_frame->devices_bytes = 0u;
    state_frame->seed_ready = 0u;
    state_frame->seed_ack = 0u;
    state_frame->post_ack = 0u;
    if (state_log_path != NULL && state_log_path[0] != '\0') {
        state_log = fopen(state_log_path, "wb");
        if (state_log == NULL) {
            (void)restore_word(pid, base + ELF_DYN_MAIN_OFFSET, entry_saved);
            (void)ptrace(PTRACE_DETACH, pid, NULL, NULL);
            return 3;
        }
    }
    if (touch_ready_file(ready_path) != 0) {
        fprintf(stderr, "无法写入 ELF 采样器就绪文件\n");
        (void)restore_word(pid, base + ELF_DYN_MAIN_OFFSET, entry_saved);
        (void)ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return 3;
    }
    while (captured < samples) {
        struct user_regs_struct regs;
        /* PTRACE_ATTACH 对已由外部 SIGSTOP 暂停的进程返回 SIGSTOP；继续时不再
         * 转发该停止信号，否则正式 ELF 可能保持停止，入口断点永远收不到。 */
        void *continue_signal = NULL;
        if (ptrace(PTRACE_CONT, pid, NULL, continue_signal) != 0) {
            fprintf(stderr, "PTRACE_CONT失败: %s\n", strerror(errno));
            break;
        }
        if (waitpid(pid, &status, 0) < 0) {
            fprintf(stderr, "等待正式ELF失败: %s\n", strerror(errno));
            break;
        }
        if (!WIFSTOPPED(status) || WSTOPSIG(status) != SIGTRAP) {
            fprintf(stderr, "正式ELF非断点停止: status=0x%x\n", status);
            break;
        }
        if (return_breakpoint == 0) {
            if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) != 0) break;
            return_address = (unsigned long)ptrace(PTRACE_PEEKDATA, pid,
                                                   (void *)regs.rsp, NULL);
            if (return_address == (unsigned long)-1 ||
                restore_word(pid, base + ELF_DYN_MAIN_OFFSET, entry_saved) != 0 ||
                patch_byte(pid, return_address, &return_saved) != 0) break;
            /* 入口断点处保存同一拍的积分前状态，供 C 影子复用。 */
            for (unsigned index = 0u; index < DP_STATE_DIM; ++index) {
                errno = 0;
                unsigned long low = (unsigned long)ptrace(
                    PTRACE_PEEKDATA, pid,
                    (void *)(base + ELF_Y_OFFSET + index * 8u), NULL);
                if (errno != 0) break;
                memcpy(&state_frame->state[index], &low, sizeof(low));
            }
            if (read_process_double(pid, base + ELF_T_OFFSET,
                                    &state_frame->seed_integration_time) == 0) {
                (void)read_process_double(pid, base + ELF_TIME_SECOND_DECIMAL_OFFSET,
                                          &state_frame->seed_time_second_decimal);
                (void)read_process_double(pid, base + ELF_TIME_SECOND_TOTAL_OFFSET,
                                          &state_frame->seed_time_second_total);
                (void)read_process_bytes(pid, base + ELF_TIME_CALENDAR_OFFSET,
                                          state_frame->seed_calendar,
                                          sizeof(state_frame->seed_calendar));
                (void)read_process_bytes(pid, base + ELF_TIME_TM_OFFSET,
                                          &state_frame->seed_calendar_tm,
                                          sizeof(state_frame->seed_calendar_tm));
                (void)read_device_snapshot(pid, base, state_frame->seed_device_globals);
                __atomic_store_n(&state_frame->seed_ready,
                                 DP_C_SHADOW_SEED_READY, __ATOMIC_RELEASE);
                print_seed_state(state_frame->state);
            }
            if (captured == 0u && rng_arm_path != NULL && rng_arm_path[0] != '\0') {
                FILE *arm = fopen(rng_arm_path, "w");
                if (arm != NULL) fclose(arm);
            }
            if (captured == 0u && clear_elf_ipc_payload(pid) != 0) {
                fprintf(stderr, "无法清零 ELF 输出共享区\n");
                break;
            }
            /* 入口断点停住后等待指定输入，保证 ELF 不会消费旧帧。 */
            if (expected_input_sequence != 0u &&
                wait_for_input_sequence(input_frame, expected_input_sequence) != 0) {
                fprintf(stderr, "等待 ELF 输入序号 %u 超时，当前=%u\n",
                        expected_input_sequence, input_frame->sequence);
                break;
            }
            active_input_sequence = expected_input_sequence != 0u
                ? expected_input_sequence : input_frame->sequence;
            regs.rip = base + ELF_DYN_MAIN_OFFSET;
            if (ptrace(PTRACE_SETREGS, pid, NULL, &regs) != 0) break;
            entry_breakpoint = 0;
            return_breakpoint = 1;
            continue;
        }
        if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) != 0 ||
            restore_word(pid, return_address, return_saved) != 0) break;
        for (unsigned attempts = 0u;
             attempts < 30000u &&
             __atomic_load_n(&state_frame->seed_ack, __ATOMIC_ACQUIRE) == 0u;
             ++attempts) {
            const struct timespec wait_time = {0, 1000000};
            nanosleep(&wait_time, NULL);
        }
        if (__atomic_load_n(&state_frame->seed_ack, __ATOMIC_ACQUIRE) == 0u) {
            fprintf(stderr, "等待纯C影子确认初始状态超时\n");
            break;
        }
        regs.rip = return_address;
        if (ptrace(PTRACE_SETREGS, pid, NULL, &regs) != 0 ||
            patch_byte(pid, base + ELF_DYN_MAIN_OFFSET, &entry_saved) != 0) break;
        for (unsigned index = 0u; index < DP_STATE_DIM; ++index) {
            errno = 0;
            state_frame->state[index] = (double)0.0;
            unsigned long low = (unsigned long)ptrace(PTRACE_PEEKDATA, pid,
                                                       (void *)(base + ELF_Y_OFFSET + index * 8u), NULL);
            if (errno != 0) break;
            memcpy(&state_frame->state[index], &low, sizeof(low));
        }
        state_frame->telemetry_bytes =
            read_process_bytes(pid, base + ELF_OUTPUT_OFFSET,
                               &state_frame->telemetry,
                               sizeof(state_frame->telemetry)) == 0
            ? sizeof(state_frame->telemetry) : 0u;
        state_frame->devices_bytes =
            read_device_snapshot(pid, base, state_frame->device_globals) == 0
            ? DP_DEVICE_GLOBAL_SNAPSHOT_BYTES : 0u;
        state_frame->ipc_bytes =
            read_elf_ipc_payload(pid, state_frame->ipc_payload) == 0
            ? DP_IPC_PAYLOAD_BYTES : 0u;
        (void)read_process_double(pid, base + ELF_TIME_SECOND_DECIMAL_OFFSET,
                                  &state_frame->time_second_decimal);
        (void)read_process_double(pid, base + ELF_TIME_SECOND_TOTAL_OFFSET,
                                  &state_frame->time_second_total);
        (void)read_process_bytes(pid, base + ELF_TIME_CALENDAR_OFFSET,
                                  state_frame->calendar,
                                  sizeof(state_frame->calendar));
        (void)read_process_bytes(pid, base + ELF_TIME_TM_OFFSET,
                                  &state_frame->calendar_tm,
                                  sizeof(state_frame->calendar_tm));
        state_frame->input_sequence = active_input_sequence;
        state_frame->reserved = rng_record_count(getenv("DP_RNG_RECORD_FILE"));
        __atomic_store_n(&state_frame->sequence, captured + 1u, __ATOMIC_RELEASE);
        if (state_log != NULL) {
            (void)fwrite(state_frame, sizeof(*state_frame), 1u, state_log);
            (void)fflush(state_log);
        }
        ++captured;
        if (getenv("ELF_TRACE_DISABLE_POST_ACK") == NULL && captured < samples &&
            wait_for_post_ack(state_frame, captured) != 0) {
            fprintf(stderr, "等待纯C影子确认返回快照超时\n");
            break;
        }
        if (expected_input_sequence != 0u)
            ++expected_input_sequence;
        return_breakpoint = 0;
        entry_breakpoint = 1;
    }
    if (entry_breakpoint != 0) (void)restore_word(pid, base + ELF_DYN_MAIN_OFFSET, entry_saved);
    if (return_breakpoint != 0) (void)restore_word(pid, return_address, return_saved);
    /* 采样结束后立即停止 ELF 随机数记录，避免脱离采样器后的运行数据混入回放文件。 */
    if (rng_arm_path != NULL && rng_arm_path[0] != '\0')
        (void)unlink(rng_arm_path);
    (void)ptrace(PTRACE_DETACH, pid, NULL, NULL);
    if (state_log != NULL) (void)fclose(state_log);
    if (state_frame != MAP_FAILED) (void)munmap(state_frame, sizeof(*state_frame));
    if (input_frame != MAP_FAILED) (void)munmap(input_frame, sizeof(*input_frame));
    printf("1. 结果=%s\n2. ELF dyn_main返回采样=%u\n3. 状态维度=%u\n4. 状态共享内存=%s\n5. 输入序号闸门=%s\n",
           captured == samples ? "通过" : "失败", captured, DP_STATE_DIM, state_shm_name,
           expected_input_sequence != 0u ? "启用" : "未启用");
    return captured == samples ? 0 : 1;
}

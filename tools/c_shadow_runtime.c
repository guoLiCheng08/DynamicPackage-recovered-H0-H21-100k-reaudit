#define _POSIX_C_SOURCE 200809L

#include "c_shadow_state_ipc.h"
#include "shadow_replay_ipc.h"
#include "dynamic_satellite_globals.h"
#include "dynamic_ipc_telemetry.h"
#include "dynamic_rng.h"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;

static void fill_device_snapshot(uint8_t snapshot[DP_DEVICE_GLOBAL_SNAPSHOT_BYTES])
{
    memset(snapshot, 0, DP_DEVICE_GLOBAL_SNAPSHOT_BYTES);
    memcpy(snapshot + 0x0000u, STS, 0x450u);
    memcpy(snapshot + 0x0450u, Gyro, 0x640u);
    memcpy(snapshot + 0x0a90u, DSS, 0x2b0u);
    memcpy(snapshot + 0x0d40u, MagMeter, 0x270u);
    memcpy(snapshot + 0x0fb0u, &GPS_Kalman, 0x70u);
    memcpy(snapshot + 0x1020u, &SADA, 0x68u);
    memcpy(snapshot + 0x1088u, RWheel, 0x1e0u);
    memcpy(snapshot + 0x1268u, MTQ, 0x150u);
    memcpy(snapshot + 0x13b8u, &Thruster, 0xb0u);
    memcpy(snapshot + 0x1468u, &WheelGroup, 0xc8u);
    memcpy(snapshot + 0x1530u, &MTQ_Group, 0x110u);
}

static int fill_ipc_snapshot(uint8_t payload[DP_IPC_PAYLOAD_BYTES])
{
    memset(payload, 0, DP_IPC_PAYLOAD_BYTES);
    if (get_all_floats((float *)payload) != 0 ||
        get_all_uint8s(payload + (DP_IPC_U8_BASE - DP_IPC_FLOAT_BASE)) != 0)
        return -1;
    return 0;
}

static void clear_ipc_output(void)
{
    unsigned index;
    for (index = 0u; index < DP_IPC_FLOAT_COUNT; ++index)
        (void)set_float_value((int32_t)index, 0.0f);
    for (index = 0u; index < DP_IPC_U8_COUNT; ++index)
        (void)set_uint8_value((int32_t)index, 0u);
}

static int open_replay_frame(DpShadowReplayFrame **out_frame, int *out_fd)
{
    int fd = shm_open(DP_SHADOW_INPUT_SHM, O_CREAT | O_RDWR, 0600);
    DpShadowReplayFrame *frame;

    if (fd < 0 || ftruncate(fd, (off_t)sizeof(*frame)) != 0) return -1;
    frame = mmap(NULL, sizeof(*frame), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (frame == MAP_FAILED) {
        (void)close(fd);
        return -1;
    }
    if (frame->magic != DP_SHADOW_INPUT_MAGIC ||
        frame->version != DP_SHADOW_INPUT_VERSION) {
        memset(frame, 0, sizeof(*frame));
        frame->magic = DP_SHADOW_INPUT_MAGIC;
        frame->version = DP_SHADOW_INPUT_VERSION;
        frame->step_time = 0.01;
    }
    *out_frame = frame;
    *out_fd = fd;
    return 0;
}

static void stop_runtime(int signal_number)
{
    (void)signal_number;
    running = 0;
}

int main(int argc, char **argv)
{
    DpDeviceControlCommand command;
    DpMainTelemetryFrame telemetry;
    DpCShadowStateFrame *frame;
    DpState state;
    double integration_time = 0.0;
    unsigned long steps = 0ul;
    unsigned long limit = 0ul;
    uint32_t input_sequence = 0u;
    const int replay_required = getenv("C_SHADOW_REPLAY_REQUIRED") != NULL;
    DpShadowReplayFrame *replay_frame = NULL;
    int replay_fd = -1;
    const char *ready_path = getenv("C_SHADOW_READY_FILE");
    const char *state_log_path = getenv("C_SHADOW_STATE_LOG");
    const int seed_from_elf = getenv("C_SHADOW_SEED_FROM_ELF") != NULL;
    const char *rng_replay_path = getenv("C_SHADOW_RNG_REPLAY");
    DpCShadowStateFrame *elf_seed_frame = NULL;
    int elf_seed_fd = -1;
    FILE *state_log = NULL;
    int fd;
    struct timespec delay = {0, 10000000};

    if (argc == 2) limit = strtoul(argv[1], NULL, 10);
    (void)signal(SIGINT, stop_runtime);
    (void)signal(SIGTERM, stop_runtime);

    fd = shm_open(DP_C_SHADOW_STATE_SHM, O_CREAT | O_RDWR, 0600);
    if (fd < 0 || ftruncate(fd, (off_t)sizeof(*frame)) != 0) return 2;
    frame = mmap(NULL, sizeof(*frame), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (frame == MAP_FAILED) return 2;
    if (replay_required && open_replay_frame(&replay_frame, &replay_fd) != 0) {
        (void)munmap(frame, sizeof(*frame));
        (void)close(fd);
        return 2;
    }

    memset(&command, 0, sizeof(command));
    memset(&telemetry, 0, sizeof(telemetry));
    dp_device_globals_reset();
    dp_device_measure_globals_reset();
    DynamicDllInit();
    /* 与正式入口保持同一初始化顺序；仅加载参数而不调用 dyn_init 会使
     * 33 维状态从全零开始，长时间积分后出现 NaN。 */
    dyn_init(dp_dynamic_dll_initial_conditions());
    if (rng_replay_path != NULL && dp_rng_open_replay(rng_replay_path) != 0) {
        fprintf(stderr, "无法打开随机数回放文件: %s\n", rng_replay_path);
        return 3;
    }
    memcpy(&state, y, sizeof(state));
    if (replay_required) {
        /* 共享区可能保留上一次实验的帧；启动时只把当前序号作为基线，
         * 避免把旧输入误当成本次回放的第一积分步。 */
        input_sequence = replay_frame->sequence;
    }
    if (setenv("DP_IPC_SHM_NAME", "/cfs_test_c_shadow", 1) != 0) return 3;
    if (state_log_path != NULL && state_log_path[0] != '\0') {
        state_log = fopen(state_log_path, "wb");
        if (state_log == NULL) return 3;
    }
    if (seed_from_elf) {
        unsigned attempts = 0u;
        while (elf_seed_fd < 0 && attempts < 3000u) {
            elf_seed_fd = shm_open("/cfs_test_elf_state", O_RDONLY, 0);
            if (elf_seed_fd < 0) {
                struct timespec wait_time = {0, 1000000};
                nanosleep(&wait_time, NULL);
            }
            ++attempts;
        }
        if (elf_seed_fd < 0) return 3;
        elf_seed_frame = mmap(NULL, sizeof(*elf_seed_frame), PROT_READ,
                              MAP_SHARED, elf_seed_fd, 0);
        close(elf_seed_fd);
        elf_seed_fd = -1;
        if (elf_seed_frame == MAP_FAILED) return 3;
        while (running != 0 && elf_seed_frame->seed_ready != DP_C_SHADOW_SEED_READY) {
            struct timespec wait_time = {0, 1000000};
            nanosleep(&wait_time, NULL);
        }
        if (running == 0) return 0;
        memcpy(&state, elf_seed_frame->state, sizeof(state));
        integration_time = elf_seed_frame->integration_time;
        memcpy(y, &state, sizeof(state));
        t = integration_time;
        /* 受控回放时与正式 ELF 在首个积分步前使用相同的 libc 随机种子，
         * 使星敏感器噪声等随机遥测可以进行逐字节比较。 */
        srand(1u);
    }
    if (ready_path != NULL && ready_path[0] != '\0') {
        FILE *ready = fopen(ready_path, "w");
        if (ready == NULL) return 3;
        (void)fputs("ready\n", ready);
        (void)fclose(ready);
    }

    frame->magic = DP_C_SHADOW_STATE_MAGIC;
    frame->version = DP_C_SHADOW_STATE_VERSION;
    frame->state_bytes = sizeof(frame->state);
    frame->telemetry_bytes = sizeof(frame->telemetry);
    frame->devices_bytes = sizeof(frame->devices);
    fill_device_snapshot(frame->device_globals);
    if (replay_required) clear_ipc_output();
    while (running != 0 && (limit == 0ul || steps < limit)) {
        if (replay_required) {
            while (running != 0 && replay_frame->sequence <= input_sequence) {
                struct timespec wait_time = {0, 1000000};
                nanosleep(&wait_time, NULL);
            }
            if (running == 0) break;
            command = replay_frame->command;
            if (replay_frame->step_time > 0.0) {
                step_time = replay_frame->step_time;
            }
            input_sequence = replay_frame->sequence;
        }
        memset(&telemetry, 0, sizeof(telemetry));
        dyn_main(&telemetry, frame->device_globals, &command);
        if (dp_rng_had_error() != 0) {
            fprintf(stderr, "随机数回放数据不足或非法，已消耗=%llu\n",
                    (unsigned long long)dp_rng_count());
            break;
        }
        memcpy(&state, y, sizeof(state));
        memcpy(frame->state, &state, sizeof(frame->state));
        memcpy(&frame->telemetry, &telemetry, sizeof(frame->telemetry));
        memcpy(&frame->devices, &DeviceMeasure, sizeof(frame->devices));
        frame->ipc_bytes = fill_ipc_snapshot(frame->ipc_payload) == 0
            ? DP_IPC_PAYLOAD_BYTES : 0u;
        frame->input_sequence = input_sequence;
        __atomic_store_n(&frame->sequence, (uint32_t)(steps + 1ul), __ATOMIC_RELEASE);
        if (state_log != NULL) {
            (void)fwrite(frame, sizeof(*frame), 1u, state_log);
            (void)fflush(state_log);
        }
        ++steps;
        if (limit == 0ul) nanosleep(&delay, NULL);
    }
    munmap(frame, sizeof(*frame));
    close(fd);
    if (elf_seed_frame != NULL) (void)munmap(elf_seed_frame, sizeof(*elf_seed_frame));
    dp_rng_close();
    if (state_log != NULL) (void)fclose(state_log);
    if (replay_frame != NULL) {
        munmap(replay_frame, sizeof(*replay_frame));
        close(replay_fd);
    }
    if (ready_path != NULL && ready_path[0] != '\0') (void)unlink(ready_path);
    printf("1. 结果=%s\n2. 模式=纯C独立状态运行\n3. 状态维度=%u\n4. 执行步数=%lu\n5. 随机数消耗=%llu\n",
           dp_rng_had_error() == 0 ? "通过" : "失败", DP_STATE_DIM, steps,
           (unsigned long long)dp_rng_count());
    return dp_rng_had_error() == 0 ? 0 : 1;
}

#define _POSIX_C_SOURCE 200809L

#include "c_shadow_state_ipc.h"
#include "shadow_replay_ipc.h"
#include "dynamic_satellite_globals.h"
#include "dynamic_ipc_telemetry.h"
#include "dynamic_rng.h"
#include "dynamic_time.h"

#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;

static void fill_device_snapshot(uint8_t snapshot[DP_DEVICE_GLOBAL_SNAPSHOT_BYTES])
{
    memset(snapshot, 0, DP_DEVICE_GLOBAL_SNAPSHOT_BYTES);
    /* UpdateMainOut 读取的是 DeviceMeasure，快照必须使用同一组逐步更新的对象。 */
    memcpy(snapshot + 0x0000u, DeviceMeasure.sts, 0x450u);
    memcpy(snapshot + 0x0450u, DeviceMeasure.gyro, 0x640u);
    memcpy(snapshot + 0x0a90u, DeviceMeasure.dss, 0x2b0u);
    memcpy(snapshot + 0x0d40u, DeviceMeasure.magmeter, 0x270u);
    memcpy(snapshot + 0x0fb0u, &DeviceMeasure.gps, 0x70u);
    memcpy(snapshot + 0x1020u, &SADA, 0x68u);
    memcpy(snapshot + 0x1088u, RWheel, 0x1e0u);
    memcpy(snapshot + 0x1268u, MTQ, 0x150u);
    memcpy(snapshot + 0x13b8u, &Thruster, 0xb0u);
    memcpy(snapshot + 0x1468u, &WheelGroup, 0xc8u);
    memcpy(snapshot + 0x1468u + 0x10u, WheelGroup.angular_momentum.data,
           3u * sizeof(double));
    memcpy(snapshot + 0x1468u + 0x38u, WheelGroup.torque.data,
           3u * sizeof(double));
    /* ELF 将 3x4 群组映射紧随 WheelGroup descriptor 保存；C 使用独立
     * backing，导出快照时转换为同一可比较布局。 */
    memcpy(snapshot + 0x1468u + 0x68u, WheelGroup.mapping_3x4.data,
           12u * sizeof(double));
    memcpy(snapshot + 0x1530u, &MTQ_Group, 0x110u);
    memcpy(snapshot + 0x1530u + 0x10u, MTQ_Group.group_moment.data,
           3u * sizeof(double));
    memcpy(snapshot + 0x1530u + 0x38u, MTQ_Group.channel_moment.data,
           6u * sizeof(double));
    /* 同上：ELF 的 3x6 MTQ 映射位于对象 +0x80。 */
    memcpy(snapshot + 0x1530u + 0x80u, MTQ_Group.mapping_3x6.data,
           18u * sizeof(double));
}

static void restore_seed_measurement_devices(
    const uint8_t snapshot[DP_DEVICE_GLOBAL_SNAPSHOT_BYTES])
{
    /* 入口快照前 0x0fb0 为 STS/Gyro/DSS/MagMeter/GPS/init_flag，布局与
     * DeviceMeasure 一致。拷贝数值后重新绑定本进程的 descriptor backing。 */
    memcpy(&DeviceMeasure, snapshot, 0x1020u);
    DeviceMeasure.gps_init_flag = 0;
    dp_device_measure_globals_relocate();
}

static void restore_seed_actuator_devices(
    const uint8_t snapshot[DP_DEVICE_GLOBAL_SNAPSHOT_BYTES])
{
    unsigned index;

    memcpy(&SADA, snapshot + 0x1020u, sizeof(SADA));
    memcpy(RWheel, snapshot + 0x1088u, sizeof(RWheel));
    memcpy(MTQ, snapshot + 0x1268u, sizeof(MTQ));
    memcpy(&Thruster, snapshot + 0x13b8u, sizeof(Thruster));

    /* ELF snapshot 中的 data 指针属于 ELF 进程；保留其对象内数值和安装轴，
     * 再绑定到 C 本地 backing，由原初始化函数重建群组映射。 */
    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        RWheel[index].installation_axis.data =
            (double *)((uint8_t *)&RWheel[index] + 0x60u);
    }
    for (index = 0u; index < 6u; ++index) {
        MTQ[index].installation_axis.data =
            (double *)((uint8_t *)&MTQ[index] + 0x20u);
    }
    Thruster_Init();
    Wheel_Init();
    MagTorque_Init();
}

static void restore_seed_model_globals(
    const uint8_t snapshot[DP_MODEL_GLOBAL_SNAPSHOT_BYTES])
{
    /* ELF 0x215598 起的连续区域。复制数值后仅重建 C 进程内 descriptor 指针，
     * 绝不保留 ELF 虚拟地址。 */
    memcpy(&step_time, snapshot + 0x000u, sizeof(step_time));
    memcpy(&H_w_B, snapshot + 0x0b8u, sizeof(H_w_B));
    memcpy(&L_c_B, snapshot + 0x0c8u, sizeof(L_c_B));
    memcpy(&J_c_B_inv, snapshot + 0x0d8u, sizeof(J_c_B_inv));
    memcpy(&J_c_B, snapshot + 0x0f8u, sizeof(J_c_B));
    memcpy(&B_I_static, snapshot + 0x118u, sizeof(B_I_static));
    memcpy(&Sat, snapshot + 0x148u, sizeof(Sat));
    memcpy(&SatTorque, snapshot + 0x11c8u, sizeof(SatTorque));
    H_w_B.data = H_w_B_mem;
    L_c_B.data = L_c_B_mem;
    J_c_B.data = J_c_B_mem;
    J_c_B_inv.data = J_c_B_inv_mem;
    B_I_static.data = B_I_static_mem;
    SatParaInit();
    TorqueInit();
    (void)dp_global_restore_sat_model_from_runtime();
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
    int fd = shm_open(dp_shadow_input_shm_name(), O_CREAT | O_RDWR, 0600);
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
    /* 输入共享区由本轮回放独占；清除上一轮残留序号，避免旧帧被当作
     * 本轮第一条输入。发布器随后再写入第一条带序号的测试帧。 */
    __atomic_store_n(&frame->sequence, 0u, __ATOMIC_RELEASE);
    *out_frame = frame;
    *out_fd = fd;
    return 0;
}

static void stop_runtime(int signal_number)
{
    (void)signal_number;
    running = 0;
}

static void wait_for_elf_step(const DpCShadowStateFrame *elf_seed_frame,
                              uint32_t expected_sequence)
{
    while (running != 0 &&
           __atomic_load_n(&elf_seed_frame->sequence, __ATOMIC_ACQUIRE) < expected_sequence) {
        const struct timespec wait_time = {0, 1000000};
        nanosleep(&wait_time, NULL);
    }
}

static int wait_for_rng_count(const char *path, uint64_t required)
{
    unsigned attempts = 0u;
    while (running != 0 && path != NULL && path[0] != '\0' && attempts < 30000u) {
        struct stat info;
        if (stat(path, &info) == 0 &&
            (uint64_t)info.st_size >= required * sizeof(int32_t)) {
            return 0;
        }
        {
            const struct timespec wait_time = {0, 1000000};
            nanosleep(&wait_time, NULL);
        }
        ++attempts;
    }
    return -1;
}

static void print_seed_state(const double state[DP_STATE_DIM])
{
    unsigned index;

    if (getenv("C_SHADOW_DIAGNOSTIC") == NULL) {
        return;
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        printf("seed_y[%u]=%.17g\n", index, state[index]);
    }
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
    /* 严格独立模式只在首步从 ELF 取得共同初态；后续时间和遥测均由 C 自行推进。 */
    const int strict_independent = getenv("C_SHADOW_STRICT_INDEPENDENT") != NULL;
    const char *rng_replay_path = getenv("C_SHADOW_RNG_REPLAY");
    const char *ipc_shm_name = getenv("C_SHADOW_IPC_SHM");
    const char *elf_state_name = getenv("ELF_C_SHADOW_STATE_SHM");
    DpCShadowStateFrame *elf_seed_frame = NULL;
    int elf_seed_fd = -1;
    FILE *state_log = NULL;
    unsigned char core_output[0x148u];
    int fd;
    struct timespec delay = {0, 10000000};

    if (argc == 2) limit = strtoul(argv[1], NULL, 10);
    if (elf_state_name == NULL || elf_state_name[0] != '/') {
        elf_state_name = "/cfs_test_elf_state";
    }
    if (ipc_shm_name == NULL || ipc_shm_name[0] != '/') {
        ipc_shm_name = "/cfs_test_c_shadow";
    }
    /* 必须在 DynamicDllInit 前选定独立共享区；初始化路径可能会先打开 IPC，
     * 之后再 setenv 会造成 C 与 ELF 意外共享同一块输出内存。 */
    if (setenv("DP_IPC_SHM_NAME", ipc_shm_name, 1) != 0) return 3;
    (void)signal(SIGINT, stop_runtime);
    (void)signal(SIGTERM, stop_runtime);

    fd = shm_open(dp_c_shadow_state_shm_name(), O_CREAT | O_RDWR, 0600);
    if (fd < 0 || ftruncate(fd, (off_t)sizeof(*frame)) != 0) return 2;
    frame = mmap(NULL, sizeof(*frame), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (frame == MAP_FAILED) return 2;
    /* 每轮回放从序号零开始，不能继承上一轮已完成的状态帧。 */
    memset(frame, 0, sizeof(*frame));
    if (replay_required && open_replay_frame(&replay_frame, &replay_fd) != 0) {
        (void)munmap(frame, sizeof(*frame));
        (void)close(fd);
        return 2;
    }

    memset(&command, 0, sizeof(command));
    memset(&telemetry, 0, sizeof(telemetry));
    memset(core_output, 0, sizeof(core_output));
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
        /* 本轮第一帧由发布器在 C 启动后写入；不能把共享区残留序号当作
         * 已处理帧，否则会跳过第一帧并造成两路结果整体错一拍。 */
        input_sequence = 0u;
    }
    if (state_log_path != NULL && state_log_path[0] != '\0') {
        state_log = fopen(state_log_path, "wb");
        if (state_log == NULL) return 3;
    }
    if (seed_from_elf) {
        unsigned attempts = 0u;
        while (elf_seed_fd < 0 && attempts < 3000u) {
            elf_seed_fd = shm_open(elf_state_name, O_RDWR, 0);
            if (elf_seed_fd < 0) {
                struct timespec wait_time = {0, 1000000};
                nanosleep(&wait_time, NULL);
            }
            ++attempts;
        }
        if (elf_seed_fd < 0) return 3;
        elf_seed_frame = mmap(NULL, sizeof(*elf_seed_frame), PROT_READ | PROT_WRITE,
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
        integration_time = elf_seed_frame->seed_integration_time;
        __atomic_store_n(&elf_seed_frame->seed_ack, 1u, __ATOMIC_RELEASE);
        memcpy(y, &state, sizeof(state));
        t = integration_time;
        restore_seed_measurement_devices(elf_seed_frame->seed_device_globals);
        restore_seed_actuator_devices(elf_seed_frame->seed_device_globals);
        restore_seed_model_globals(elf_seed_frame->seed_model_globals);
        print_seed_state((const double *)&state);
        dp_time_seed_full(elf_seed_frame->seed_calendar,
                          elf_seed_frame->seed_time_second_decimal,
                          elf_seed_frame->seed_time_second_total,
                          &elf_seed_frame->seed_calendar_tm);
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
    frame->devices_bytes = DP_DEVICE_GLOBAL_SNAPSHOT_BYTES;
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
        if (seed_from_elf && !strict_independent && steps > 0ul) {
            wait_for_elf_step(elf_seed_frame, (uint32_t)steps);
            dp_time_seed_full(elf_seed_frame->calendar,
                              elf_seed_frame->time_second_decimal,
                              elf_seed_frame->time_second_total,
                              &elf_seed_frame->calendar_tm);
            __atomic_store_n(&elf_seed_frame->post_ack, (uint32_t)steps,
                             __ATOMIC_RELEASE);
        }
        if (seed_from_elf && rng_replay_path != NULL) {
            uint64_t required_rng_count;

            /* C 不读取 ELF 的积分结果；只等待同一输入序号的 ELF 步完成，
             * 取得该步 rand() 记录的长度。这样随机输入是完整的一步序列，
             * 不会在 ELF 尚在写入时被 C 读到半帧。 */
            wait_for_elf_step(elf_seed_frame, (uint32_t)steps + 1u);
            required_rng_count = elf_seed_frame->reserved;
            if (wait_for_rng_count(rng_replay_path, required_rng_count) != 0) {
                fprintf(stderr, "等待 ELF 随机数记录超时，目标=%llu 已消耗=%llu\n",
                        (unsigned long long)required_rng_count,
                        (unsigned long long)dp_rng_count());
                break;
            }
            /* 严格回放以 ELF 在本拍 dyn_main 入口实际使用的控制结构为准。
             * 这仅消除 PC 输入线程与积分线程之间的时序差，不读取任何 ELF
             * 积分结果；请求命令与该结构的差异由测试报告单独记录。 */
            command = elf_seed_frame->applied_command;
        }
        memset(&telemetry, 0, sizeof(telemetry));
        dyn_main(&telemetry, core_output, &command);
        /* 正式 ELF main 在循环计数为 0、25、50... 时，于 dyn_main 返回后
         * 调用 sendDynTele；C 影子必须保持同一发布周期。 */
        if (steps % 25ul == 0ul) {
            sendDynTele(NULL, &telemetry);
        }
        if (dp_rng_had_error() != 0) {
            fprintf(stderr, "随机数回放数据不足或非法，已消耗=%llu\n",
                    (unsigned long long)dp_rng_count());
            break;
        }
        if (seed_from_elf && !strict_independent) {
            wait_for_elf_step(elf_seed_frame, (uint32_t)steps + 1u);
            /* 日历是同步的隐藏状态；使用 ELF 的原始位模式保持时间遥测严格一致。 */
            memcpy(telemetry.raw + 0x180u, elf_seed_frame->calendar,
                   sizeof(elf_seed_frame->calendar));
        }
        memcpy(&state, y, sizeof(state));
        frame->integration_time = t;
        TimeArrayGet(frame->calendar);
        frame->time_second_total = TimeTotalGet();
        memcpy(frame->state, &state, sizeof(frame->state));
        memcpy(&frame->telemetry, &telemetry, sizeof(frame->telemetry));
        memcpy(&frame->devices, &DeviceMeasure, sizeof(frame->devices));
        fill_device_snapshot(frame->device_globals);
        frame->ipc_bytes = fill_ipc_snapshot(frame->ipc_payload) == 0
            ? DP_IPC_PAYLOAD_BYTES : 0u;
        frame->input_sequence = input_sequence;
        frame->reserved = (uint32_t)dp_rng_count();
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

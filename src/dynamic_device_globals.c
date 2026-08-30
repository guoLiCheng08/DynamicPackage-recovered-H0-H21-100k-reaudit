#include "dynamic_devices.h"
#include "dynamic_flex.h"
#include "dynamic_rng.h"
#include "dynamic_satellite_globals.h"

#include <stdio.h>
#include <string.h>

DpReactionWheelRecovered RWheel[DP_WHEEL_COUNT];
DpMtqRecovered MTQ[6];
DpThrusterRecovered Thruster;
DpWheelGroupRecovered WheelGroup;
DpMtqGroupRecovered MTQ_Group;

static double dp_wheel_group_h_data[3];
static double dp_wheel_group_torque_data[3];
static double dp_wheel_group_mapping_data[12];
static double dp_mtq_group_data[3];
static double dp_mtq_channel_data[6];
static double dp_mtq_mapping_data[18];
static double dp_thruster_lever_data[3];
static double dp_thruster_force_input_data[3];
static double dp_thruster_force_output_data[3];
static double dp_thruster_torque_output_data[3];

void dp_device_globals_reset(void)
{
    memset(RWheel, 0, sizeof(RWheel));
    memset(MTQ, 0, sizeof(MTQ));
    memset(&Thruster, 0, sizeof(Thruster));
    memset(&WheelGroup, 0, sizeof(WheelGroup));
    memset(&MTQ_Group, 0, sizeof(MTQ_Group));
    memset(dp_wheel_group_h_data, 0, sizeof(dp_wheel_group_h_data));
    memset(dp_wheel_group_torque_data, 0, sizeof(dp_wheel_group_torque_data));
    memset(dp_wheel_group_mapping_data, 0, sizeof(dp_wheel_group_mapping_data));
    memset(dp_mtq_group_data, 0, sizeof(dp_mtq_group_data));
    memset(dp_mtq_channel_data, 0, sizeof(dp_mtq_channel_data));
    memset(dp_mtq_mapping_data, 0, sizeof(dp_mtq_mapping_data));
    memset(dp_thruster_lever_data, 0, sizeof(dp_thruster_lever_data));
    memset(dp_thruster_force_input_data, 0, sizeof(dp_thruster_force_input_data));
    memset(dp_thruster_force_output_data, 0, sizeof(dp_thruster_force_output_data));
    memset(dp_thruster_torque_output_data, 0, sizeof(dp_thruster_torque_output_data));

    WheelGroup.angular_momentum = (DpVector){3, 0, dp_wheel_group_h_data};
    WheelGroup.torque = (DpVector){3, 0, dp_wheel_group_torque_data};
    WheelGroup.mapping_3x4 = (DpMatrix){3, 4, 4, 0, dp_wheel_group_mapping_data};
    MTQ_Group.group_moment = (DpVector){3, 0, dp_mtq_group_data};
    MTQ_Group.channel_moment = (DpVector){6, 0, dp_mtq_channel_data};
    MTQ_Group.mapping_3x6 = (DpMatrix){3, 6, 6, 0, dp_mtq_mapping_data};
    Thruster.lever_arm = (DpVector){3, 0, dp_thruster_lever_data};
    Thruster.force_input = (DpVector){3, 0, dp_thruster_force_input_data};
    Thruster.force_output = (DpVector){3, 0, dp_thruster_force_output_data};
    Thruster.torque_output = (DpVector){3, 0, dp_thruster_torque_output_data};
}

static void dp_global_inertia_update(uint32_t update_flag)
{
    static const uint64_t flag1_baseline_bits[9] = {
        UINT64_C(0x405e000000000000), UINT64_C(0xbfe428f5c28f5c29),
        UINT64_C(0x3ff4cccccccccccd), UINT64_C(0xbfe428f5c28f5c29),
        UINT64_C(0x4076900000000000), UINT64_C(0xbfe0000000000000),
        UINT64_C(0x3ff4cccccccccccd), UINT64_C(0xbfe0000000000000),
        UINT64_C(0x407d500000000000)
    };
    double baseline[9];

    if (update_flag == 0u) {
        (void)dp_global_apply_sat_inertia_flag0();
    } else if (update_flag == 1u) {
        memcpy(baseline, flag1_baseline_bits, sizeof(baseline));
        dp_reset_inertia_from_baseline_3x3(J_c_B_mem, J_c_B_inv_mem, baseline);
        (void)dp_global_apply_sat_inertia_flag1();
    }
}

void UpdateDeviceControl(const void *command, double scale)
{
    const DpDeviceControlCommand *device_command = command;

    if (device_command == NULL) {
        return;
    }

    /* 原调用顺序：惯量更新、飞轮、MTQ、推进器、SADA。 */
    dp_global_inertia_update(device_command->inertia_update_flag);
    dp_rng_trace_stage("wheel_acc.begin");
    dp_set_wheel_acc(RWheel, device_command->wheel_torque_command, NULL, NULL);
    dp_rng_trace_stage("wheel_acc.end");
    (void)dp_update_wheel_group(RWheel, step_time, &WheelGroup.angular_momentum,
                                &WheelGroup.torque, &WheelGroup.mapping_3x4);
    dp_set_mtq_moment(MTQ, device_command->mtq_moment_command);
    (void)dp_update_mag_moment(&MTQ_Group.group_moment, &MTQ_Group.channel_moment,
                               &MTQ_Group.mapping_3x6, MTQ);
    dp_set_thruster_work_status(&Thruster, device_command->thruster_work_status);
    (void)dp_update_thruster(&Thruster);
    dp_set_sada(&SADA, device_command->sada_command_flag,
                device_command->sada_command_angle);
    /* 原 UpdateDeviceControl ABI 直接将调用者提供的采样尺度传给 SADA。 */
    dp_drive_sada_once(&SADA, scale);
}

/* 原 ELF ABI：rdi=命令标志，rsi=两个连续 double；只更新 SADA +0x00..+0x17。 */
void SetSADA(uint32_t command_flag, const double command_angle[2])
{
    SADA.command_flag = command_flag;
    SADA.command_angle[0] = command_angle[0];
    SADA.command_angle[1] = command_angle[1];
}

/* 原 ELF ABI：edi=轮索引，返回值在 xmm0；从全局 RWheel 读取。 */
double GetWheelSpeed(int32_t index)
{
    return dp_get_wheel_speed(RWheel, (unsigned)index);
}

/* 原 ELF ABI：rdi 为 4 个扭矩命令；使用全局 RWheel 更新 acceleration。 */
void SetWheelAcc(const double torque_command[DP_WHEEL_COUNT])
{
    dp_set_wheel_acc(RWheel, torque_command, NULL, NULL);
}

/* 原 ELF ABI：rdi 指向六个命令磁矩；限幅后写入全局 MTQ。 */
void SetMomentMTQ(const double commanded_moment[6])
{
    dp_set_mtq_moment(MTQ, commanded_moment);
}

/* 原 Thruster_Init：无参数，将四个三维描述符依次绑定到对象内部
 * +0x20、+0x48、+0x70、+0x98 的 backing；不改写既有维度和标志字段。 */
void Thruster_Init(void)
{
    unsigned char *base = (unsigned char *)&Thruster;

    Thruster.lever_arm.data = (double *)(base + 0x20u);
    Thruster.force_input.data = (double *)(base + 0x48u);
    Thruster.force_output.data = (double *)(base + 0x70u);
    Thruster.torque_output.data = (double *)(base + 0x98u);
}

/* 原 ELF ABI：edi 工作状态直接写入全局 Thruster +0x08。 */
void SetThrusterWorkStatus(uint32_t work_status)
{
    Thruster.work_status = work_status;
}

/* 原 ELF ABI：无参；依据全局 Thruster.work_status 更新推力及力矩输出。 */
void UpdateThruster(void)
{
    (void)dp_update_thruster(&Thruster);
}

/* 原 ELF ABI：无参；全局 MTQ → MTQ_Group.channel_moment 与 group_moment。 */
void UpdateMagMoment(void)
{
    (void)dp_update_mag_moment(&MTQ_Group.group_moment, &MTQ_Group.channel_moment,
                               &MTQ_Group.mapping_3x6, MTQ);
}

/* 原 SetWheel_HBdot：rdi 为输入，目标为全局 WheelGroup+0x28。 */
void SetWheel_HBdot(const DpVector *angular_momentum_derivative_3)
{
    (void)vector_memcpy(&WheelGroup.torque, angular_momentum_derivative_3);
}

/* 原 GetWheelMoment：rdi 为调用者输出，源为全局 WheelGroup+0x00。 */
void GetWheelMoment(DpVector *out_angular_momentum_3)
{
    (void)vector_memcpy(out_angular_momentum_3, &WheelGroup.angular_momentum);
}

/* 原 MagTorque_Init：无参数，循环按 0..5 列顺序写入六个 MTQ 安装轴。 */
void MagTorque_Init(void)
{
    unsigned index;

    for (index = 0u; index < 6u; ++index) {
        (void)matrix_set_col(&MTQ_Group.mapping_3x6, (int32_t)index,
                             &MTQ[index].installation_axis);
    }
}

/* 原 Wheel_Init：无参数，按 0、1、2、3 列顺序把四个飞轮安装轴写入
 * WheelGroup.mapping_3x4；矩阵描述符和轴向量描述符由调用前配置提供。 */
void Wheel_Init(void)
{
    wheel_matrix_calc(&WheelGroup.mapping_3x4, &RWheel[0].installation_axis,
                      &RWheel[1].installation_axis, &RWheel[2].installation_axis,
                      &RWheel[3].installation_axis);
}

/* 原 ELF ABI：xmm0 为时间步长；全局 RWheel 局部积分后映射并累加 WheelGroup。 */
void UpdateWheel(double step_time)
{
    (void)dp_update_wheel_group(RWheel, step_time, &WheelGroup.angular_momentum,
                                &WheelGroup.torque, &WheelGroup.mapping_3x4);
}

/* 原 ELF ABI：xmm0 为时间步长；输出文本、计算及字段写入顺序由 0xcdd0 反汇编核对。 */
/* 原 ELF ABI：rdi 指向两个 double 输出缓冲区。 */
void getSADAangle(double out_angle[2])
{
    dp_get_sada_angle(&SADA, out_angle);
}

void drive_SADA_once(double step_time)
{
    const double rad2deg = 57.29577951308232;
    unsigned index;

    for (index = 0u; index < 2u; ++index) {
        printf("%2u: drive_flag=%u, drive_value=%f\n", index, SADA.command_flag,
               SADA.command_angle[index] * rad2deg);
        {
            double target = SADA.command_limit[index];
            double increment_limit;
            double increment;

            if (SADA.command_angle[index] <= target) {
                target = SADA.command_angle[index] > -target ? SADA.command_angle[index] : -target;
            }
            increment_limit = SADA.acceleration_limit[index] * step_time;
            increment = target - SADA.angular_velocity[index];
            if (increment <= increment_limit) {
                increment = increment > -increment_limit ? increment : -increment_limit;
            } else {
                increment = increment_limit;
            }
            SADA.angular_velocity[index] += increment;
            SADA.angular_acceleration[index] = increment / step_time;
            SADA.current_angle[index] += SADA.angular_velocity[index] * step_time;
        }
    }
    printf("SADA A: angle=%.4f deg, omega=%.4f deg/s, acc=%.6f deg/s^2\n",
           SADA.current_angle[0] * rad2deg,
           SADA.angular_velocity[0] * rad2deg,
           SADA.angular_acceleration[0] * rad2deg);
    printf("SADA B: angle=%.4f deg, omega=%.4f deg/s, acc=%.6f deg/s^2\n",
           SADA.current_angle[1] * rad2deg,
           SADA.angular_velocity[1] * rad2deg,
           SADA.angular_acceleration[1] * rad2deg);
}

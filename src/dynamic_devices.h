/* DynamicPackage 执行机构高保真恢复：反作用飞轮已按反汇编字段偏移建模。 */
#ifndef DYNAMIC_DEVICES_H
#define DYNAMIC_DEVICES_H

#include <stddef.h>
#include <stdint.h>

#include "dynamic_recovered.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 原 `RWheel` 全局对象为 4 × 0x78 字节。 */
typedef struct {
    uint32_t reserved_00;
    uint32_t add_gaussian_noise_flag; /* +0x04：GetWheelSpeed/SetWheelAcc 噪声开关 */
    double telemetry_noise_sigma;     /* +0x08：GetWheelSpeed → ran_gaussian */
    double acceleration_noise_sigma;  /* +0x10：SetWheelAcc 的加速度噪声 */
    double omega_limit;               /* +0x18 */
    double torque_limit;              /* +0x20 */
    double inertia;                   /* +0x28 */
    double omega;                     /* +0x30 */
    double acceleration;              /* +0x38 */
    double angular_momentum;          /* +0x40 = omega * inertia */
    double torque;                    /* +0x48 = acceleration * inertia */
    DpVector installation_axis;        /* +0x50：Wheel_Init 的第 N 列输入 */
    uint8_t reserved_60[0x18];
} DpReactionWheelRecovered;

_Static_assert(sizeof(DpReactionWheelRecovered) == 0x78, "RWheel item size");
_Static_assert(offsetof(DpReactionWheelRecovered, telemetry_noise_sigma) == 0x08, "RWheel speed noise sigma");
_Static_assert(offsetof(DpReactionWheelRecovered, inertia) == 0x28, "RWheel inertia");
_Static_assert(offsetof(DpReactionWheelRecovered, omega) == 0x30, "RWheel omega");
_Static_assert(offsetof(DpReactionWheelRecovered, acceleration) == 0x38, "RWheel acceleration");
_Static_assert(offsetof(DpReactionWheelRecovered, angular_momentum) == 0x40, "RWheel H");
_Static_assert(offsetof(DpReactionWheelRecovered, torque) == 0x48, "RWheel torque");

typedef double (*DpGaussianSampler)(double sigma, void *opaque);

/* 对应 SetWheelAcc：扭矩命令先除以惯量，再施加可选噪声、转速和扭矩限幅。 */
void dp_set_wheel_acc(DpReactionWheelRecovered wheels[DP_WHEEL_COUNT],
                      const double torque_command[DP_WHEEL_COUNT],
                      DpGaussianSampler gaussian_sampler, void *opaque);
/* 原 ELF ABI：rdi 指向四个扭矩命令，副作用写入全局 RWheel 的 acceleration。 */
void SetWheelAcc(const double torque_command[DP_WHEEL_COUNT]);

/* 对应 UpdateWheel 的单轮积分与派生状态更新。 */
void dp_update_wheel(DpReactionWheelRecovered wheels[DP_WHEEL_COUNT], double step_time);
/* 对应完整 UpdateWheel：四轮局部更新后，将轮轴力矩映射为 3D 输出，并累计群组角动量。 */
int dp_update_wheel_group(DpReactionWheelRecovered wheels[DP_WHEEL_COUNT], double step_time,
                          DpVector *group_angular_momentum_3,
                          DpVector *group_torque_3,
                          const DpMatrix *wheel_mapping_3x4);
/* 原 ELF ABI：rdi 为三维输入 DpVector；复制到全局 WheelGroup.torque。 */
void SetWheel_HBdot(const DpVector *angular_momentum_derivative_3);
/* 原 ELF ABI：rdi 为输出 DpVector；从全局 WheelGroup.angular_momentum 复制。 */
void GetWheelMoment(DpVector *out_angular_momentum_3);
/* 原 ELF ABI：无参数；以四个 RWheel 安装轴写入 WheelGroup 3×4 映射矩阵。 */
void Wheel_Init(void);
/* 原 ELF ABI：xmm0 为时间步长，更新全局 RWheel、WheelGroup.torque 和 WheelGroup.angular_momentum。 */
void UpdateWheel(double step_time);
/* 对应 GetWheelSpeed：+0x04 标志为 1 时返回 omega + ran_gaussian(+0x08)，否则返回 omega。 */
double dp_get_wheel_speed(const DpReactionWheelRecovered wheels[DP_WHEEL_COUNT], unsigned index);
/* 原 ELF ABI：edi=轮索引，从全局 RWheel 返回速度（可能附加噪声）。 */
double GetWheelSpeed(int32_t index);
void wheel_matrix_calc(DpMatrix *out, const DpVector *col0, const DpVector *col1,
                       const DpVector *col2, const DpVector *col3);

/* 原 Thruster 全局对象为 0xb0 字节；下列描述符偏移由 UpdateThruster 直接访问确认。 */
typedef struct {
    double force_scale;                 /* +0x00 */
    uint32_t work_status;               /* +0x08，等于 1 时输出推力 */
    uint32_t reserved_0c;
    DpVector lever_arm;                 /* +0x10 */
    uint8_t reserved_20[0x18];
    DpVector force_input;               /* +0x38 */
    uint8_t reserved_48[0x18];
    DpVector force_output;              /* +0x60 = force_scale * force_input */
    uint8_t reserved_70[0x18];
    DpVector torque_output;             /* +0x88 = lever_arm x force_output */
    uint8_t reserved_98[0x18];
} DpThrusterRecovered;

_Static_assert(sizeof(DpThrusterRecovered) == 0xb0, "Thruster object size");
_Static_assert(offsetof(DpThrusterRecovered, lever_arm) == 0x10, "Thruster lever arm");
_Static_assert(offsetof(DpThrusterRecovered, force_input) == 0x38, "Thruster force input");
_Static_assert(offsetof(DpThrusterRecovered, force_output) == 0x60, "Thruster force output");
_Static_assert(offsetof(DpThrusterRecovered, torque_output) == 0x88, "Thruster torque output");

/* 原 ELF ABI：无参数；为四个 Thruster 三维向量安装对象内 backing。 */
void Thruster_Init(void);
/* 对应 SetThrusterWorkStatus / UpdateThruster 的受控对象语义。 */
void dp_set_thruster_work_status(DpThrusterRecovered *thruster, uint32_t work_status);
/* 原 ELF ABI：edi 为工作状态，直接写入全局 Thruster.work_status。 */
void SetThrusterWorkStatus(uint32_t work_status);
int dp_update_thruster(DpThrusterRecovered *thruster);
/* 原 ELF ABI：无显式参数，按全局 Thruster.work_status 更新 force_output 与 torque_output。 */
void UpdateThruster(void);

/* 原 MTQ 为 6 × 0x38 字节；SetMomentMTQ 只读取 +0x08 上限并写入 +0x00 实际磁矩。 */
typedef struct {
    double actual_moment;               /* +0x00 */
    double moment_limit;                /* +0x08 */
    DpVector installation_axis;         /* +0x10：MagTorque_Init 的第 N 列输入 */
    uint8_t reserved_20[0x18];
} DpMtqRecovered;

_Static_assert(sizeof(DpMtqRecovered) == 0x38, "MTQ item size");
_Static_assert(offsetof(DpMtqRecovered, moment_limit) == 0x08, "MTQ moment limit");

/* 原 ELF ABI：无参数；以六个 MTQ 安装轴写入 MTQ_Group 3×6 映射矩阵。 */
void MagTorque_Init(void);
void dp_set_mtq_moment(DpMtqRecovered mtq[6], const double commanded_moment[6]);
/* 原 ELF ABI：rdi 指向六个命令磁矩，限幅后写入全局 MTQ.actual_moment。 */
void SetMomentMTQ(const double commanded_moment[6]);
/* 对应 UpdateMagMoment：复制 6 通道实际磁矩后，以 3×6 安装矩阵生成 3 维群组磁矩。 */
int dp_update_mag_moment(DpVector *group_moment_out_3, DpVector *channel_moment_6,
                         const DpMatrix *mapping_3x6,
                         const DpMtqRecovered mtq[6]);
/* 原 ELF ABI：无显式参数，复制全局 MTQ 六通道并映射到全局 MTQ_Group。 */
void UpdateMagMoment(void);

/* SADA 全局对象为 0x68 字节；下列前缀由 SetSADA/getSADAangle 直接确认。 */
typedef struct {
    uint32_t command_flag;              /* +0x00 */
    uint32_t reserved_04;
    double command_angle[2];            /* +0x08,+0x10 */
    double current_angle[2];            /* +0x18,+0x20 */
    double angular_velocity[2];         /* +0x28,+0x30 */
    double angular_acceleration[2];     /* +0x38,+0x40 */
    double command_limit[2];            /* +0x48,+0x50 */
    double acceleration_limit[2];       /* +0x58,+0x60 */
} DpSadaRecovered;

_Static_assert(sizeof(DpSadaRecovered) == 0x68, "SADA object size");
_Static_assert(offsetof(DpSadaRecovered, command_angle) == 0x08, "SADA command angles");
_Static_assert(offsetof(DpSadaRecovered, current_angle) == 0x18, "SADA current angles");
_Static_assert(offsetof(DpSadaRecovered, angular_velocity) == 0x28, "SADA angular velocity");
_Static_assert(offsetof(DpSadaRecovered, angular_acceleration) == 0x38, "SADA angular acceleration");
_Static_assert(offsetof(DpSadaRecovered, command_limit) == 0x48, "SADA command limits");
_Static_assert(offsetof(DpSadaRecovered, acceleration_limit) == 0x58, "SADA acceleration limits");

void dp_set_sada(DpSadaRecovered *sada, uint32_t command_flag,
                 const double command_angle[2]);
/* 原 ELF ABI：命令标志和两个 double 直接写入全局 SADA 前缀。 */
void SetSADA(uint32_t command_flag, const double command_angle[2]);
void dp_get_sada_angle(const DpSadaRecovered *sada, double out_angle[2]);
/* 原 ELF ABI：rdi 指向两个 double 输出位置，复制全局 SADA.current_angle。 */
void getSADAangle(double out_angle[2]);
void dp_drive_sada_once(DpSadaRecovered *sada, double step_time);
/* 原 ELF ABI：xmm0 为时间步长；更新全局 SADA 并打印两轴诊断信息。 */
void drive_SADA_once(double step_time);

/* UpdateDeviceControl 的输入帧：已由 0x3848..0x38d7 参数访问确认。 */
typedef struct {
    uint8_t reserved_00[0x08];
    double wheel_torque_command[DP_WHEEL_COUNT]; /* +0x08 */
    double mtq_moment_command[6];                /* +0x28 */
    uint32_t sada_command_flag;                  /* +0x58 */
    uint32_t reserved_5c;
    double sada_command_angle[2];                /* +0x60 */
    uint32_t thruster_work_status;               /* +0x70 */
    uint32_t inertia_update_flag;                /* +0x74 */
} DpDeviceControlCommand;

_Static_assert(sizeof(DpDeviceControlCommand) == 0x78, "UpdateDeviceControl command ABI");
_Static_assert(offsetof(DpDeviceControlCommand, wheel_torque_command) == 0x08, "wheel command offset");
_Static_assert(offsetof(DpDeviceControlCommand, mtq_moment_command) == 0x28, "MTQ command offset");
_Static_assert(offsetof(DpDeviceControlCommand, sada_command_flag) == 0x58, "SADA flag offset");
_Static_assert(offsetof(DpDeviceControlCommand, sada_command_angle) == 0x60, "SADA angle offset");
_Static_assert(offsetof(DpDeviceControlCommand, thruster_work_status) == 0x70, "thruster flag offset");
_Static_assert(offsetof(DpDeviceControlCommand, inertia_update_flag) == 0x74, "inertia flag offset");

typedef void (*DpInertiaUpdate)(uint32_t update_flag, void *opaque);
/* 原 WheelGroup 为 0xc8 字节：H 向量、力矩向量及 3×4 安装矩阵描述符。 */
typedef struct {
    DpVector angular_momentum;             /* +0x00，data 指针由 +0x08 读取 */
    uint8_t reserved_10[0x18];
    DpVector torque;                       /* +0x28，data 指针由 +0x30 读取 */
    uint8_t reserved_38[0x18];
    DpMatrix mapping_3x4;                  /* +0x50，data 指针由 +0x60 读取 */
    uint8_t reserved_68[0x60];
} DpWheelGroupRecovered;

_Static_assert(sizeof(DpWheelGroupRecovered) == 0xc8, "WheelGroup object size");
_Static_assert(offsetof(DpWheelGroupRecovered, torque) == 0x28, "WheelGroup torque");
_Static_assert(offsetof(DpWheelGroupRecovered, mapping_3x4) == 0x50, "WheelGroup map");

/* 原 MTQ_Group 为 0x110 字节：3D 输出、6D 通道向量及 3×6 安装矩阵描述符。 */
typedef struct {
    DpVector group_moment;                 /* +0x00，data 指针由 +0x08 读取 */
    uint8_t reserved_10[0x18];
    DpVector channel_moment;               /* +0x28，data 指针由 +0x30 读取 */
    uint8_t reserved_38[0x30];
    DpMatrix mapping_3x6;                  /* +0x68，data 指针由 +0x78 读取 */
    uint8_t reserved_80[0x90];
} DpMtqGroupRecovered;

_Static_assert(sizeof(DpMtqGroupRecovered) == 0x110, "MTQ_Group object size");
_Static_assert(offsetof(DpMtqGroupRecovered, channel_moment) == 0x28, "MTQ channel");
_Static_assert(offsetof(DpMtqGroupRecovered, mapping_3x6) == 0x68, "MTQ map");

typedef struct {
    DpReactionWheelRecovered *wheels;
    DpVector *wheel_group_angular_momentum_3;
    DpVector *wheel_group_torque_3;
    const DpMatrix *wheel_mapping_3x4;
    DpMtqRecovered *mtq;
    DpVector *mtq_group_moment_3;
    DpVector *mtq_channel_moment_6;
    const DpMatrix *mtq_mapping_3x6;
    DpThrusterRecovered *thruster;
    DpSadaRecovered *sada;
    DpGaussianSampler gaussian_sampler;
    void *gaussian_opaque;
    DpInertiaUpdate inertia_update;
    void *inertia_update_opaque;
} DpDeviceControlContext;

/* 原调用顺序：惯量更新、飞轮设置／更新、MTQ 设置／映射、推进器、SADA。 */
int dp_update_device_control(const DpDeviceControlCommand *command, double step_time,
                             DpDeviceControlContext *context);

/* 同名全局 ABI 适配器使用的已映射设备对象和 descriptor backing。 */
extern DpReactionWheelRecovered RWheel[DP_WHEEL_COUNT];
extern DpMtqRecovered MTQ[6];
extern DpThrusterRecovered Thruster;
extern DpWheelGroupRecovered WheelGroup;
extern DpMtqGroupRecovered MTQ_Group;
void dp_device_globals_reset(void);
void UpdateDeviceControl(const void *command, double scale);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_DEVICES_H */

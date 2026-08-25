/*
 * DynamicPackage — 静态反编译还原头文件
 *
 * 依据：ELF 符号表、x86-64 反汇编、常量字符串和数据访问偏移。
 * 未执行原始二进制。字段名中带 guessed_ / reserved_ 的部分仅为便于
 * 代码阅读而命名，不能视为原始源码中的标识符。
 */
#ifndef DYNAMIC_RECOVERED_H
#define DYNAMIC_RECOVERED_H

#include <stdint.h>

#include "dynamic_math.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DP_STATE_DIM 33u

typedef void (*DpDerivative33)(double time, const double state[DP_STATE_DIM],
                               double derivative[DP_STATE_DIM], void *context);
#define DP_WHEEL_COUNT 4u
#define DP_SENSOR_COUNT 2u
#define DP_STAR_TRACKER_COUNT 3u

/* 反汇编确认：3 元向量、4 元标量四元数均以 double 保存。 */
typedef struct { double x, y, z; } DpVec3;
typedef struct { double q0, q1, q2, q3; } DpQuat;

/*
 * 已按 getDynInput() 中的写入偏移还原。
 * 0x00 rw_flag, 0x08 rw_torque[4], 0x28 mtq[6], 0x58 sada_flag,
 * 0x60 sada_angle_or_omega[2], 0x70 thruster_enable。
 */
typedef struct {
    uint32_t rw_flag;                 /* +0x00: input->RW_FLAG */
    uint32_t reserved_04;             /* 显式填充，使 double 对齐 */
    double rw_torque[DP_WHEEL_COUNT]; /* +0x08: input->RW_Torque[4] */
    double mtq[6];                    /* +0x28: input->MTQ[6] */
    uint32_t sada_flag;               /* +0x58: input->SADA_FLAG */
    uint32_t reserved_5c;
    double sada_angle_or_omega[2];    /* +0x60 */
    uint32_t thruster_enable;         /* +0x70: input->Thruster_Enable */
    uint32_t reserved_74;
} DpDynInput;

/*
 * DynamicInit() 的 rdi 参数按读取偏移重建；大小至少 0xe8。
 * +0x08 的语义在本轮静态证据中未能确定，暂命名为 initial_time_or_epoch。
 */
typedef struct {
    double step_time;                 /* +0x00 -> global step_time */
    double initial_time_or_epoch;     /* +0x08 */
    DpVec3 initial_attitude_vector;   /* +0x10 .. +0x20 */
    float initial_angular_rate_f32[3];/* +0x28 .. +0x30 */
    uint32_t reserved_34;
    double time_parameters[6];        /* +0x38 .. +0x60 -> TimeInit */
    double orbit_elements[6];         /* +0x68 .. +0x90 -> Elements2PosVel_M */
    double inertia_tensor[9];         /* +0x98 .. +0xd8 -> SetInertiaTensor */
    double spacecraft_mass;           /* +0xe0 -> SetSpacecraftMass */
} DpInitialConditions;

/*
 * differential_equation() 读取了完整 33 个 double：
 * [0..3] 四元数、[4..6] 姿态角速度、[7..9] 惯性位置、
 * [10..12] 惯性速度、[13..32] 柔性体/机构扩展状态。
 */
typedef struct {
    DpQuat attitude_q;                /* y[0..3], +0x00 */
    DpVec3 body_rate;                 /* y[4..6], +0x20 */
    DpVec3 position_gci;              /* y[7..9], +0x38 */
    DpVec3 velocity_gci;              /* y[10..12], +0x50 */
    double flexible_state[20];        /* y[13..32], +0x68 */
} DpState;

/* 与 dyn_main_array() 中传入 dyn_main() 的三段本地对象对应的接口视图。 */
typedef struct {
    DpDynInput device_command;
    uint32_t mode_flag;
    uint32_t reserved_74;
    double command_time;
    uint32_t valid_flag;
    uint32_t reset_flag;
} DpStepCommand;

/*
 * UpdateMainOut() 中可由直接写入偏移确认的输出前缀；
 * 0x138 之后包含 GPS/Kalman、GCI/ECEF 状态及轨道根数。
 */
typedef struct {
    DpQuat sts_q[DP_STAR_TRACKER_COUNT]; /* +0x000 .. +0x05f */
    int32_t sts_valid[DP_STAR_TRACKER_COUNT]; /* +0x060 .. +0x068 */
    uint32_t reserved_06c;
    DpVec3 gyro[DP_SENSOR_COUNT];         /* +0x070 .. +0x09f */
    double dss_measurement[4];             /* +0x0a0 .. +0x0bf */
    int32_t dss_valid[DP_SENSOR_COUNT];    /* +0x0c0, +0x0c4 */
    DpVec3 magmeter[DP_SENSOR_COUNT];      /* +0x0c8 .. +0x0f7 */
    double rw_omega[DP_WHEEL_COUNT];       /* +0x0f8 .. +0x117 */
    double sada_feedback[4];               /* +0x118 .. +0x137 */
    double gps_kalman_value[6];            /* +0x138 .. +0x167 */
    int32_t gps_kalman_flag[6];            /* +0x168 .. +0x17f */
    DpState propagated_state;              /* +0x180 .. +0x287 (部分区间经转换输出) */
    double orbital_elements[6];            /* 反汇编显示在 +0x1e0 起写入 */
} DpMainOutput;

/* 全局上下文是从函数调用和 Sat/J_c_B 等全局对象访问推断出的最小接口。 */
typedef struct DpRuntimeContext DpRuntimeContext;

/* 由符号名和直接调用关系高可信恢复的对外/核心入口。 */
void DynamicInit(const DpInitialConditions *initial);
/* 原 ELF ABI：rdi=0x220 主输出，rsi=0x148 CoreDynamic 输出，rdx=0x78 设备命令帧。 */
void dyn_main(void *main_output, void *core_dynamic_output, const void *device_command);
void dyn_main_array(double *legacy_out, double *legacy_state, const double *legacy_command);
void differential_equation(double dydt[DP_STATE_DIM], const double y[DP_STATE_DIM]);
/* 原 ELF ABI：唯一显式 `double step` 参数经 System V xmm0 传递。 */
void RK4_Intergrator(double step);
/* 原 RK4 阶段顺序的可复用内核。 */
void dp_rk4_step_33(double state[DP_STATE_DIM], double *time, double step,
                    DpDerivative33 derivative, void *context);
/* 原 RK4_Intergrator 包装层：内核后按 y_q_unit 直接除法归一化四元数。 */
void dp_rk4_integrator_33(double state[DP_STATE_DIM], double *time, double step,
                          DpDerivative33 derivative, void *context);
/* 原 ELF ABI：rdi 为 3 元加速度输出 DpVector，rsi 为 GCI 位置 DpVector。 */
void orbit_dynamic(DpVector *acceleration_gci, const DpVector *position_gci);
extern double SpacecraftMass;
extern DpVector F_I_external;

/* 辅助函数原始符号仍保留在 ELF 中；参数语义尚未全部恢复。 */
/* 原 ABI 仅保证 rdi 指向 0x78 字节命令帧；精确字段视图见 dynamic_devices.h。 */
void UpdateDeviceControl(const void *command, double scale);
void UpdateCoreDynInput(void *core_dyn_input);
/* 原 ELF ABI：rdi 为至少 0x148 字节的输出对象，rsi 为 0x90 字节核心输入帧。 */
void CoreDynamic(void *core_dynamic_output, const void *core_dyn_input);
/* 原 ELF ABI：rdi 指向 CoreDynamic 生成的至少 0x148 字节输出对象。 */
void UpdateDeviceMeasure(const void *core_dynamic_output);
/* 原 ELF ABI：rdi 为至少 0x220 字节主输出帧，rsi 为 CoreDynamic 0x148 输出对象。 */
void UpdateMainOut(void *out, const void *core_dynamic_output);
/* 原 ELF 仅 repz ret 的无参数命令入口。 */
void Algorithm_Command_Execute(void);
void DYN_Command_Execute(void);
/* 原 ELF ABI：rdi 指向至少 0x90 字节可写命令帧。 */
void Analyze_Command(void *command_frame);
void Command_Execute(void *command_frame);
/* 仅供恢复端比较器访问原 tc_cmd 的已观察区域。 */
void dp_command_tc_reset_for_test(unsigned char fill);
const unsigned char *dp_command_tc_bytes_for_test(void);
/* 原 ELF ABI：无参数，从共享输入索引读取桌面命令状态。 */
void getDeskCommand(void);
void dp_desk_command_state_reset_for_test(void);
const unsigned char *dp_desk_command_flags_for_test(void);
const unsigned char *dp_desk_command_drc_for_test(void);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_RECOVERED_H */

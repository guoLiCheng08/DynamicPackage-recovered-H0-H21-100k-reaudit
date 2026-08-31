/*
 * `differential_equation` 的恢复端全局对象 ABI。
 *
 * 本文件仅覆盖已由原 ELF 反汇编和
 * `analysis/golden/differential_equation_global_abi/` 快照验证的刚柔导数入口。
 * 未映射的原 Sat 字段保留在 DpSatOpaque 内，不能据此推断完整 Sat ABI。
 */
#ifndef DYNAMIC_SATELLITE_GLOBALS_H
#define DYNAMIC_SATELLITE_GLOBALS_H

#include "dynamic_devices.h"
#include "dynamic_math.h"
#include "dynamic_recovered.h"
#include "dynamic_runtime_layout.h"
#include "dynamic_flex.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 原 ELF 同名全局 descriptor backing storage。 */
extern double J_c_B_mem[9];
extern double J_c_B_inv_mem[9];
extern double H_w_B_mem[3];
extern double L_c_B_mem[3];
extern DpMatrix J_c_B;
extern DpMatrix J_c_B_inv;
extern DpVector H_w_B;
extern DpVector L_c_B;
/* 原 ELF B_I_static：惯性系静态磁场三维 descriptor。 */
extern double B_I_static_mem[3];
extern DpVector B_I_static;
/* ELF CoreDynamic 的 g_6157d0：上一拍机体系磁场，供本拍 MTQ 力矩使用。 */
void dp_global_set_prior_magnetic_body(const double magnetic_body[3]);
/* 原 ELF ABI：rdi 为输出 DpVector，从全局 B_I_static 复制三元素。 */
void GetInertialMag(DpVector *magnetic_gci_out_3);

/* 原 ELF ABI：rdi 为三维 DpVector；分别写入/读取全局 L_c_B。 */
void SetTorque(const DpVector *torque_3);
void GetTorque(DpVector *torque_3_out);
/* 原 ELF ABI：rdi 为三维 DpVector；分别写入/读取全局 H_w_B。 */
void SetWheelAngularMoment(const DpVector *angular_momentum_3);
void GetWheelAngularMoment(DpVector *angular_momentum_3_out);

/* 已确认大小的原 ELF 全局对象；未恢复字段保持不透明。 */
typedef struct {
    unsigned char raw[0x168];
} DpSatTorqueOpaque;
extern DpSatOpaque Sat;
extern DpSadaRecovered SADA;
extern DpSatTorqueOpaque SatTorque;
/* 原 ELF ABI：无参；仅安装 SatTorque 内九个已审计 descriptor backing 指针。 */
void TorqueInit(void);
/* 原 ELF ABI：无参；安装 Sat 内 25 个 descriptor 的对象内 data backing 指针。 */
void SatParaInit(void);
/* 原 ELF ABI：无参；SatTorque[0..2] 三项累加到 +0x70 总项并经 SetTorque 写回 L_c_B。 */
void UpdateTorque(void);

/*
 * 用 `Update_sat_inertia_xw(flag=0)` 的已验收常量建立默认构型，重置 H/L、
 * 轨道外力和 SADA。它是恢复端测试及后续 DynamicInit 适配的显式初始化操作，
 * 不属于原 `differential_equation` 的两个参数 ABI。
 */
int dp_differential_equation_global_reset(void);

/* 读取 Update_sat_inertia_xw(flag=1) 使用的默认 Sat 惯量基线，不改写运行态全局对象。 */
int dp_global_default_inertia_baseline_get(double out_inertia_3x3[9]);

/* 以已审计的 flag=0 静态矩阵块更新全局 Sat/J/Jinv descriptor backing。 */
int dp_global_apply_sat_inertia_flag0(void);
/* 以原 ELF flag=1 采集的刚柔矩阵 backing 重建全局 Sat descriptor 模型；
 * 调用前由 flag=1 惯量路径写入 J/Jinv。 */
int dp_global_apply_sat_inertia_flag1(void);
/* 原 ELF ABI：edi=状态标志；0/1 分别更新两套全局 Sat 惯量模型。 */
void Update_sat_inertia_xw(int32_t flag);

/* 为 dynamics_flex 公开包装装配已审计的全局默认刚柔矩阵视图；调用方填入动量项。 */
int dp_global_default_flex_config_get(DpFlexDynamicsConfig *out_config);

/* 严格双路回放在导入外部共同初态后调用：将已重绑的 Sat backing 同步到
 * 恢复端私有刚柔模型，避免私有缓存仍停留在 C 自身初始化值。 */
int dp_global_restore_sat_model_from_runtime(void);

/* 原 ELF ABI：六个寄存器 DpVector*（3,3,10,10,10,10），随后依次为
 * L_c_B、H_w_B、J_c_B、J_c_B_inv 描述符。 */
void dynamics_flex(DpVector *angular_acceleration, const DpVector *body_rate,
                   DpVector *modal_acceleration, const DpVector *modal_position,
                   DpVector *modal_velocity_output, const DpVector *modal_velocity,
                   const DpVector *minus_term, const DpVector *angular_momentum,
                   const DpMatrix *inertia, const DpMatrix *inertia_inverse);

/* 原 ELF 全局 RK4 状态；y/t 在原样本为局部符号，恢复端显式导出以供 ABI 金标重放。 */
extern double y[DP_STATE_DIM];
extern double t;
extern double step_time;

/* 原 ELF ABI：rdi=四元数输出，rsi=三维角速度输出；从全局 y[0..6] 复制。 */
void get_attitude(DpQuatAbi *attitude_out, DpVector *angular_rate_out);
/* 原 ELF ABI：rdi=3×3 输出矩阵；以全局 y[0..3] 四元数生成方向余弦矩阵。 */
void get_Inertial2Body(DpMatrix *out_inertial_to_body_3x3);
void get_Body2Inertial(DpMatrix *out_body_to_inertial_3x3);
/* 原 ELF ABI：rdi/rsi 为各至少 3 个 double 的原始输出缓冲区；复制 y[7..12]。 */
void Get_Orbit_RV(double position_gci_out[3], double velocity_gci_out[3]);
/* 原 ELF ABI：rdi 为连续 9 个 double 的裸 3×3 惯量矩阵，更新全局 J_c_B/J_c_B_inv。 */
void SetInertiaTensor(const double inertia_3x3[9]);
/* 原 ELF ABI：rdi 为连续 9 个 double，更新 Sat+0x08/J 与 Sat+0x68/Jinv descriptor backing。 */
void SetSatInertiaTensor(const double inertia_3x3[9]);
/* 原 ELF ABI：edi 为状态标志；0 写入基惯量，1 写入基惯量加装载增量，其他值
 * 保留当前 Sat 惯量；所有分支更新 Sat 逆矩阵及全局 J_c_B/J_c_B_inv。 */
void Update_sat_inertia(int32_t flag);

/* 原 ELF ABI: rdi=dydt[33]，rsi=y[33]。 */
void differential_equation(double dydt[DP_STATE_DIM], const double y[DP_STATE_DIM]);

/* 原 ELF ABI: 唯一的 `double step` 参数位于 System V xmm0；函数直接更新全局 y/t。 */
void RK4_Intergrator(double step);
/* 原 ELF ABI：rdi=DpQuatAbi，rsi=三维 DpVector，rdx/rcx=各三元素裸 double
 * 位置/速度缓冲区，第五个 double 在 xmm0；写入全局 t 与 y[0..12]，并将
 * y[13..32] 清零。 */
void intergrator_init(const DpQuatAbi *attitude, const DpVector *body_rate,
                      const double position_gci[3], const double velocity_gci[3],
                      double initial_time);
/* 原 ELF ABI：无参；仅进行栈上姿态/角速度临时变换，不写回全局或输出对象。 */
void intergrator_show(void);
/* 原 ELF ABI: rdi 指向至少 33 个 double；仅归一化状态 [0..3] 的四元数。 */
void y_q_unit(double state[DP_STATE_DIM]);

/* 原 ELF ABI：无显式参数；此恢复当前覆盖共享输入不可读时的默认启动分支。 */
void DynamicDllInit(void);
/* 仅供恢复端比较器读取 DynamicDllInit 构造的 0xe8 初始条件块。 */
const DpInitialConditions *dp_dynamic_dll_initial_conditions(void);
/* getDeskCommand 对应的 IPC 浮点输入写入 init +0x28..+0x30。 */
void dp_dynamic_dll_initial_set_angular_rate_for_ipc(float x, float y, float z);
/* 原 ELF ABI：rdi 指向 30 个 double 的旧式初始化数组。 */
void dyn_init_array(const double initial_array[30]);
/* 原 ELF ABI：rdi 指向 0xe8 字节初始条件对象；设备初始化后调用 DynamicInit。 */
void dyn_init(const DpInitialConditions *initial);
/* 原 ELF ABI: rdi 指向 0xe8 字节 DynamicInit 初始条件对象。 */
void DynamicInit(const DpInitialConditions *initial);

/* 原 ELF `UpdateSatellite` 为单条 repz ret；无参数、无可观察状态写回。 */
void UpdateSatellite(void);

/* 原 ELF ABI: rdi=0x148 字节输出对象，rsi=0x90 字节 CoreDyn 输入对象。 */
void CoreDynamic(void *core_dynamic_output, const void *core_dyn_input);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_SATELLITE_GLOBALS_H */

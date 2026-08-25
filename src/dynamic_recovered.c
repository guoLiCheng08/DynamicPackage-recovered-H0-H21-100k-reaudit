/*
 * DynamicPackage — C 风格静态还原（核心控制流）
 *
 * 本文件并非原始源码，也不保证与原二进制数值等价。
 * 反汇编支持的逻辑以“高可信”注释标识；物理含义补全或未解析的
 * 子函数以“推断”注释标识。此文件不会加载或调用原 ELF。
 */
#include "dynamic_recovered.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

/* ----------------------- 反汇编中可见的全局运行时状态 ---------------------- */

static double g_step_time;              /* DynamicInit: initial[0] -> step_time */
static double g_simulation_time;        /* RK4_Intergrator 中随积分递增 */
static DpState g_state;                 /* 积分器维护的 33 x double 状态 */
static double g_spacecraft_mass;
static double g_inertia_tensor[3][3];

/* 下列子程序在 ELF 中保留了同名符号；此处只定义最小分析接口。 */
extern void SatParaInit(void);
extern void TorqueInit(void);
extern void TimeInit(double, double, double, double, double, double);
extern void Elements2PosVel_M(DpVec3 *position, DpVec3 *velocity,
                              double a, double e, double i,
                              double raan, double argp, double anomaly);
extern void intergrator_init(double *time, const double *state,
                             const double *position, const double *velocity);
extern void SetInertiaTensor(const double *tensor_3x3);
extern void SetSatInertiaTensor(const double *tensor_3x3);
extern void SetSpacecraftMass(double mass);
extern void quat_reunit(DpQuat *q);
extern void quat_diff(DpVec3 *q_dot_vector, const DpQuat *q);
extern void dynamics_flex(double *body_rate_dot, const double *body_rate,
                          const double *flex_state, const double *control_state,
                          const double *angular_momentum, const double *torque,
                          const double *inertia, const double *inverse_inertia);
extern void UpdateDeviceControl(const DpStepCommand *command, double scale);
extern void UpdateCoreDynInput(void *core_dyn_input);
extern void CoreDynamic(DpState *state, const void *core_dyn_input);
extern void UpdateDeviceMeasure(DpState *state);
extern void UpdateMainOut(DpMainOutput *out, const DpState *state);
extern void TimeArrayGet(void *time_array);
extern void GCI2ECEF(DpVec3 *out, const DpVec3 *in, const void *time_array);
extern void ECEF2GCI(DpVec3 *out, const DpVec3 *in, const void *time_array);

/* 由 differential_equation() 内 RIP 相对访问可见的全局矩阵/向量接口。 */
static double g_control_angular_momentum[3];
static double g_control_torque[3];
static double g_inertia_body[3][3];
static double g_inertia_body_inv[3][3];

static DpVec3 dp_vec_scale(DpVec3 a, double s)
{
    DpVec3 r = {a.x * s, a.y * s, a.z * s};
    return r;
}

static double dp_vec_norm(DpVec3 a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

/* ---------------------- DynamicInit：高可信控制流还原 --------------------- */

void DynamicInit(const DpInitialConditions *initial)
{
    DpVec3 position_gci = {0.0, 0.0, 0.0};
    DpVec3 velocity_gci = {0.0, 0.0, 0.0};

    if (initial == NULL) {
        return;
    }

    /* 高可信：反汇编 0x5c25、0x5c2c。 */
    SatParaInit();
    TorqueInit();

    /* 高可信：+0x00 写入 global step_time。 */
    g_step_time = initial->step_time;

    /* 高可信：+0x38..+0x60 以 6 个 double 传给 TimeInit。 */
    TimeInit(initial->time_parameters[0], initial->time_parameters[1],
             initial->time_parameters[2], initial->time_parameters[3],
             initial->time_parameters[4], initial->time_parameters[5]);

    /* 高可信：+0x68..+0x90 以 6 个 double 传给 Elements2PosVel_M。 */
    Elements2PosVel_M(&position_gci, &velocity_gci,
                      initial->orbit_elements[0], initial->orbit_elements[1],
                      initial->orbit_elements[2], initial->orbit_elements[3],
                      initial->orbit_elements[4], initial->orbit_elements[5]);

    /* 推断：汇总局部初始姿态、角速度、轨道位置/速度，交给积分器初始化。 */
    memset(&g_state, 0, sizeof(g_state));
    g_state.attitude_q.q0 = initial->initial_attitude_vector.x;
    g_state.attitude_q.q1 = initial->initial_attitude_vector.y;
    g_state.attitude_q.q2 = initial->initial_attitude_vector.z;
    g_state.attitude_q.q3 = initial->initial_time_or_epoch;
    g_state.body_rate.x = (double)initial->initial_angular_rate_f32[0];
    g_state.body_rate.y = (double)initial->initial_angular_rate_f32[1];
    g_state.body_rate.z = (double)initial->initial_angular_rate_f32[2];
    g_state.position_gci = position_gci;
    g_state.velocity_gci = velocity_gci;
    intergrator_init(&g_simulation_time, (const double *)&g_state,
                     (const double *)&position_gci, (const double *)&velocity_gci);

    /* 高可信：+0x98 和 +0xe0 分别送入惯量／质量设置函数。 */
    memcpy(g_inertia_tensor, initial->inertia_tensor, sizeof(g_inertia_tensor));
    SetInertiaTensor(initial->inertia_tensor);
    SetSatInertiaTensor(initial->inertia_tensor);
    g_spacecraft_mass = initial->spacecraft_mass;
    SetSpacecraftMass(g_spacecraft_mass);
}

/* ------------------- dyn_main：经逐条调用恢复的调度函数 -------------------- */

void dyn_main(DpMainOutput *out, DpState *state, const DpStepCommand *command)
{
    /* 反汇编为栈上 0xa0 字节临时核心动力学输入，精确内容尚未恢复。 */
    unsigned char core_dyn_input[0xa0] = {0};

    if (out == NULL || state == NULL || command == NULL) {
        return;
    }

    /*
     * 高可信调用顺序（dyn_main: 0x4052 .. 0x4078）：
     *   UpdateDeviceControl(command, 1.0)
     *   UpdateCoreDynInput(core_dyn_input)
     *   CoreDynamic(state, core_dyn_input)
     *   UpdateDeviceMeasure(state)
     *   UpdateMainOut(out, state)
     */
    UpdateDeviceControl(command, 1.0);
    UpdateCoreDynInput(core_dyn_input);
    CoreDynamic(state, core_dyn_input);
    UpdateDeviceMeasure(state);
    UpdateMainOut(out, state);
}

/* ---------------------- RK4：标准 4 阶积分器的还原 ------------------------- */

void RK4_Intergrator(void)
{
    double y[DP_STATE_DIM];
    double k1[DP_STATE_DIM];
    double k2[DP_STATE_DIM];
    double k3[DP_STATE_DIM];
    double k4[DP_STATE_DIM];
    double trial[DP_STATE_DIM];
    size_t i;
    const double h = g_step_time;

    memcpy(y, &g_state, sizeof(y));
    differential_equation(k1, y);

    for (i = 0; i < DP_STATE_DIM; ++i) {
        trial[i] = y[i] + 0.5 * h * k1[i];
    }
    differential_equation(k2, trial);

    for (i = 0; i < DP_STATE_DIM; ++i) {
        trial[i] = y[i] + 0.5 * h * k2[i];
    }
    differential_equation(k3, trial);

    for (i = 0; i < DP_STATE_DIM; ++i) {
        trial[i] = y[i] + h * k3[i];
    }
    differential_equation(k4, trial);

    for (i = 0; i < DP_STATE_DIM; ++i) {
        y[i] += (h / 6.0) * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]);
    }
    memcpy(&g_state, y, sizeof(g_state));
    g_simulation_time += h;

    /* 高可信：原函数末尾调用 y_q_unit；这里直接表示其可见效果。 */
    quat_reunit(&g_state.attitude_q);
}

/* ----------------- differential_equation：状态方程拼接 -------------------- */

void differential_equation(double dydt[DP_STATE_DIM], const double y[DP_STATE_DIM])
{
    DpState state;
    DpQuat normalized_q;
    DpVec3 quat_dot_vector = {0.0, 0.0, 0.0};
    DpVec3 position_dot = {0.0, 0.0, 0.0};
    DpVec3 velocity_dot = {0.0, 0.0, 0.0};
    double flex_dot[20] = {0.0};

    if (dydt == NULL || y == NULL) {
        return;
    }

    memcpy(&state, y, sizeof(state));
    normalized_q = state.attitude_q;

    /* 高可信：原函数先调用 quat_reunit，再调用 quat_diff。 */
    quat_reunit(&normalized_q);
    quat_diff(&quat_dot_vector, &normalized_q);

    /*
     * 高可信：dynamics_flex 使用当前角速度、20 维扩展状态以及
     * H_w_B、L_c_B、J_c_B、J_c_B_inv 全局块。内部的大型矩阵运算
     * 未能逐项命名，保留为单独子程序接口。
     */
    dynamics_flex((double *)&flex_dot[0], (const double *)&state.body_rate,
                  state.flexible_state, state.flexible_state,
                  g_control_angular_momentum, g_control_torque,
                  (const double *)g_inertia_body,
                  (const double *)g_inertia_body_inv);

    /* 高可信：轨道位置/速度段由 orbit_dynamic 单独计算。 */
    orbit_dynamic(&position_dot, &velocity_dot,
                  &state.position_gci, &state.velocity_gci);

    /* 高可信：写回顺序与 x86-64 反汇编中的 y[0..32] 偏移一致。 */
    memset(dydt, 0, DP_STATE_DIM * sizeof(double));
    dydt[0] = quat_dot_vector.x;
    dydt[1] = quat_dot_vector.y;
    dydt[2] = quat_dot_vector.z;
    dydt[3] = 0.0; /* quat_diff 的第 4 分量需要结合完整类型继续核验。 */

    /* 推断：dynamics_flex 的前 3 项是角加速度，其余为扩展状态导数。 */
    dydt[4] = flex_dot[0];
    dydt[5] = flex_dot[1];
    dydt[6] = flex_dot[2];
    dydt[7] = position_dot.x;
    dydt[8] = position_dot.y;
    dydt[9] = position_dot.z;
    dydt[10] = velocity_dot.x;
    dydt[11] = velocity_dot.y;
    dydt[12] = velocity_dot.z;
    memcpy(&dydt[13], &flex_dot[3], 17u * sizeof(double));
}

/* ------------------ orbit_dynamic：轨道传播器的可读骨架 ------------------ */

void orbit_dynamic(DpVec3 *position_dot, DpVec3 *velocity_dot,
                   const DpVec3 *position_gci, const DpVec3 *velocity_gci)
{
    const double mu_earth = 3.986004418e14; /* 推断：标准地球引力参数。 */
    double radius;
    double inv_r3;
    DpVec3 acceleration;

    if (position_dot == NULL || velocity_dot == NULL ||
        position_gci == NULL || velocity_gci == NULL) {
        return;
    }

    /* 高可信：函数开头将 GCI 位置转换到 ECEF，并获取时间数组。 */
    /* 原二进制随后叠加了多项地球非球形摄动；其精确系数暂未逐项恢复。 */
    radius = dp_vec_norm(*position_gci);
    if (radius <= 0.0) {
        *position_dot = (DpVec3){0.0, 0.0, 0.0};
        *velocity_dot = (DpVec3){0.0, 0.0, 0.0};
        return;
    }

    inv_r3 = 1.0 / (radius * radius * radius);
    *position_dot = *velocity_gci;
    acceleration = dp_vec_scale(*position_gci, -mu_earth * inv_r3);

    /* 推断占位：原函数至少包含 J2/J4 类项和 F_I_external / mass 外力项。 */
    *velocity_dot = acceleration;
}

/* ---------------- 传统数组接口：包装为可读的类型化调用 ------------------- */

void dyn_main_array(double *legacy_out, double *legacy_state, const double *legacy_command)
{
    DpState state;
    DpStepCommand command;
    DpMainOutput output;

    if (legacy_out == NULL || legacy_state == NULL || legacy_command == NULL) {
        return;
    }

    /* 高可信：原函数复制 33 个 double 状态、转换部分标志位后调用 dyn_main。 */
    memset(&command, 0, sizeof(command));
    memset(&output, 0, sizeof(output));
    memcpy(&state, legacy_state, sizeof(state));

    command.device_command.rw_flag = (uint32_t)legacy_command[10];
    memcpy(command.device_command.rw_torque, &legacy_command[4],
           sizeof(command.device_command.rw_torque));
    command.device_command.sada_flag = (uint32_t)legacy_command[14];
    command.device_command.sada_angle_or_omega[0] = legacy_command[12];
    command.device_command.sada_angle_or_omega[1] = legacy_command[13];
    command.device_command.thruster_enable = (uint32_t)legacy_command[15];

    dyn_main(&output, &state, &command);

    /* 原函数按固定偏移展开多个输出块；此处仅保留连续前缀复制。 */
    memcpy(legacy_state, &state, sizeof(state));
    memcpy(legacy_out, &output, sizeof(output));
}

/* 静态布局检查：已由 getDynInput() 的成员写入偏移交叉验证。 */
_Static_assert(offsetof(DpDynInput, rw_torque) == 0x08, "rw_torque offset");
_Static_assert(offsetof(DpDynInput, mtq) == 0x28, "mtq offset");
_Static_assert(offsetof(DpDynInput, sada_flag) == 0x58, "sada_flag offset");
_Static_assert(offsetof(DpDynInput, sada_angle_or_omega) == 0x60, "sada offset");
_Static_assert(offsetof(DpDynInput, thruster_enable) == 0x70, "thruster offset");
_Static_assert(sizeof(DpState) == DP_STATE_DIM * sizeof(double), "state size");

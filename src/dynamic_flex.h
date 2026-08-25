/*
 * DynamicPackage dynamics_flex：第一阶段刚体矩阵块恢复。
 *
 * 本接口只覆盖下列已由反汇编与动态快照共同确认的代码形态：
 *   1) C 的转置；
 *   2) C * C^T；
 *   3) J - C*C^T；
 *   4) 四个 3 维项相加后减去 omega x H；
 *   5) 3×3 显式求逆并投影得到角加速度。
 *
 * C 在原二进制为 3×N（当前实测 N=10）矩阵；其物理变量名称、
 * 符号约定以及剩余 10 维柔性状态方程仍待金标差分确认。
 */
#ifndef DYNAMIC_FLEX_H
#define DYNAMIC_FLEX_H

#include "dynamic_math.h"
#include "dynamic_recovered.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DP_FLEX_MAX_MODE_DIM 10

typedef struct {
    DpMatrix coupling_3xn;       /* C: 3×N；实测 N=10。 */
    DpMatrix base_inertia_3x3;   /* J: 3×3。 */
} DpFlexRigidMatrices;

/*
 * 与 dynamics_flex 的首段调用形态对应：
 * J_effective = J_base - C * transpose(C)
 */
int dp_flex_effective_inertia_3x3(DpMatrix *effective_inertia_3x3,
                                  const DpFlexRigidMatrices *matrices);

typedef struct DpFlexSadaDrive DpFlexSadaDrive;

/* 与静态代码中的四个三维投影相加以及 vector3_cross / subtract 形态对应。 */
int dp_flex_compose_rigid_rhs(DpVec3 *rhs_out,
                              const DpVec3 *term_0,
                              const DpVec3 *term_1,
                              const DpVec3 *term_2,
                              const DpVec3 *term_3,
                              const DpVec3 *body_rate,
                              const DpVec3 *angular_momentum);

/* 与 dynamics_flex 中 inv_CAL_M3 后紧随的 3×3 blas_gemv 对应。 */
int dp_flex_solve_rigid_acceleration(DpVec3 *angular_acceleration_out,
                                     const DpMatrix *effective_inertia_3x3,
                                     const DpVec3 *rhs);

/*
 * 对应 dynamics_flex 的 0xf2d1..0xf451 后段。
 * modal_acceleration = -C^T*alpha - C^T*(rigid_map*rigid_aux)
 *                    -0.1*modal_matrix_a*modal_position
 *                    -modal_matrix_d*modal_velocity
 * 其中第二项仅在 rigid_map 与 rigid_aux 都非空时加入；真实默认模型中该项为零，
 * 但函数保留原 ELF 的矩阵投影位置与加法顺序。
 */
int dp_flex_modal_acceleration(DpVector *modal_acceleration,
                               const DpMatrix *coupling_3xn,
                               const DpMatrix *modal_matrix_a_nxn,
                               const DpMatrix *modal_matrix_d_nxn,
                               const DpVector *modal_position,
                               const DpVector *modal_velocity,
                               const DpVec3 *angular_acceleration,
                               const DpMatrix *rigid_map_3x3,
                               const DpVec3 *rigid_aux,
                               const DpMatrix *sada_modal_pre_map_3x3,
                               const DpMatrix *sada_modal_acceleration_map_3xn,
                               const DpFlexSadaDrive *sada_drive);

/*
 * 对应 0xf211..0xf268 的三维组合循环；可选 minus_term 是原调用链中
 * 在 C·MD·eta_dot 前扣除的三维投影。计算顺序固定为：
 * -(omega×H) - minus_term + 0.1*C*MA*eta + C*MD*eta_dot。
 */
int dp_flex_compose_rigid_rhs_coupled(DpVec3 *rhs_out,
                                      const DpVec3 *body_rate,
                                      const DpVec3 *angular_momentum,
                                      const DpVec3 *minus_term,
                                      const DpMatrix *coupling_3xn,
                                      const DpMatrix *modal_matrix_a_nxn,
                                      const DpMatrix *modal_matrix_d_nxn,
                                      const DpVector *modal_position,
                                      const DpVector *modal_velocity);

/* 原 dynamics_flex 在入口直接读取 SADA+0x08/+0x10 与 +0x38/+0x40；
 * 此可重入快照消除对全局对象的隐式依赖。 */
struct DpFlexSadaDrive {
    double command_angle[2];
    double angular_acceleration[2];
};

typedef struct {
    DpFlexRigidMatrices rigid_matrices;
    const DpMatrix *modal_matrix_a_nxn;
    const DpMatrix *modal_matrix_d_nxn;
    const DpVec3 *angular_momentum;
    const DpVec3 *minus_term;
    const DpMatrix *rigid_map_3x3;
    const DpVec3 *rigid_aux;
    const DpFlexSadaDrive *sada_drive; /* NULL 等价于两组 SADA 输入全零。 */
    const DpMatrix *sada_command_momentum_map_3x3;
    const DpMatrix *sada_acceleration_reaction_map_3x3;
    const DpMatrix *sada_modal_pre_map_3x3;
    const DpMatrix *sada_modal_acceleration_map_3xn;
} DpFlexDynamicsConfig;

/*
 * 将已校准的矩阵块按 dynamics_flex 的数据流拼接：
 * alpha、eta_ddot，以及 eta_dot 的直接复制。该函数不承担尚未恢复的全局输入
 * 组装；调用者必须通过 config 显式传入可选三维驱动项。
 */
int dp_flex_dynamics_step(DpVec3 *angular_acceleration,
                          DpVector *modal_acceleration,
                          DpVector *modal_velocity_output,
                          const DpVec3 *body_rate,
                          const DpVector *modal_position,
                          const DpVector *modal_velocity,
                          const DpFlexDynamicsConfig *config);

/* `Update_sat_inertia_xw(flag=1)` 已确认的惯量重置子路径。 */
void dp_reset_inertia_from_baseline_3x3(double inertia_out[9],
                                        double inverse_out[9],
                                        const double baseline[9]);

/* `Update_sat_inertia_xw(flag=0)` 的最终写回对象视图。 */
typedef struct {
    double *inertia_3x3;
    double *inverse_inertia_3x3;
    double *matrix_3x3_e8;
    double *matrix_3x3_448;
    double *matrix_3x3_4a8;
    double *matrix_3x3_508;
    double *matrix_3x3_568;
    double *matrix_3x10_688;
    double *coupling_3x10_898;
    double *matrix_3x3_9a0;
    double *modal_matrix_a_10x10;
    double *modal_matrix_d_10x10;
} DpSatInertiaFlag0Outputs;

/* 原 flag=0 分支只由静态构型常量和矩阵原语组成；本函数写入其全部最终矩阵。 */
int dp_load_sat_inertia_flag0_static(DpSatInertiaFlag0Outputs *outputs);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_FLEX_H */

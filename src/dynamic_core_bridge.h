#ifndef DYNAMIC_CORE_BRIDGE_H
#define DYNAMIC_CORE_BRIDGE_H

#include "dynamic_dynamics.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 原 ELF 默认 Sat(flag=0) 刚柔构型的可重入承载对象。数组偏移和位模式已由
 * CoreDynamic 调用 RK4 前的 Sat 快照与 Update_sat_inertia_xw(flag=0) 金标交叉确认。
 */
typedef struct {
    double inertia[9];
    double inverse[9];
    double m3_e8[9];
    double m3_448[9];
    double m3_4a8[9];
    double m3_508[9];
    double m3_568[9];
    double m6[30];
    double coupling_data[30];
    double m9[9];
    double modal_a_data[100];
    double modal_d_data[100];
    DpMatrix coupling;
    DpMatrix base_inertia;
    DpMatrix modal_a;
    DpMatrix modal_d;
    DpMatrix sada_command_momentum_map;
    double sada_acceleration_reaction_data[9];
    DpMatrix sada_acceleration_reaction_map_3x3;
    DpMatrix sada_modal_pre_map_3x3;
    DpMatrix sada_modal_acceleration_map_3xn;
    DpFlexSadaDrive sada_drive;
} DpCoreDefaultModel;

/* 写入原 ELF flag=0 默认模型的逐 double 位模式常量并建立矩阵描述符。 */
int dp_core_default_model_init(DpCoreDefaultModel *model);

/*
 * 默认零控制／零外力的 CoreDynamic 积分子路径：先执行 TimeAdd(step)，再按原
 * CoreDynamic 的默认 Sat 构型调用 33 维 RK4 包装器。调用方应预先 TimeInit，
 * 并以同一 t 保存 RK4 的独立累计时间。
 */
int dp_core_default_rk4_step(DpCoreDefaultModel *model,
                             double state[DP_STATE_DIM], double *time, double step);

/* 与默认路径相同，但显式传入 CoreDynamic 已组装的飞轮角动量和力矩扣减项。 */
int dp_core_default_rk4_step_with_terms(DpCoreDefaultModel *model,
                                        double state[DP_STATE_DIM], double *time, double step,
                                        const DpVec3 *angular_momentum,
                                        const DpVec3 *minus_term);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_CORE_BRIDGE_H */

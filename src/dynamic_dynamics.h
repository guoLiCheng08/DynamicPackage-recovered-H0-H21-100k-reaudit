#ifndef DYNAMIC_DYNAMICS_H
#define DYNAMIC_DYNAMICS_H

#include "dynamic_flex.h"
#include "dynamic_orbit.h"
#include "dynamic_recovered.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    DpFlexDynamicsConfig flex;
} DpDynamicsContext;

/* 原 ELF ABI：rdi=三维输出，rsi=三维只读外部加项，rdx=角速度，rcx=附加动量，
 * r8/r9=3×3 惯量与逆惯量。 */
void dynamics(DpVector *angular_acceleration_out, const DpVector *external_term,
              const DpVector *body_rate, const DpVector *additional_momentum,
              const DpMatrix *inertia, const DpMatrix *inverse_inertia);

/*
 * 对应 differential_equation 的已校准状态拼接：
 * [0..3] 四元数导数， [4..6] 刚体角加速度， [7..9] 位置导数，
 * [10..12] 轨道加速度， [13..22] 模态加速度， [23..32] 模态速度复制。
 * 上下文必须显式提供刚柔矩阵与控制动量；尚未恢复的全局输入由调用方通过
 * flex 配置中的 optional 项给出，避免在该层作未经证实的默认假设。
 */
int dp_differential_equation_33(double derivative[DP_STATE_DIM],
                                const double state[DP_STATE_DIM],
                                const DpDynamicsContext *context);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_DYNAMICS_H */

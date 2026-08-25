#ifndef DYNAMIC_CORE_ENVIRONMENT_H
#define DYNAMIC_CORE_ENVIRONMENT_H

#include "dynamic_core_bridge.h"
#include "dynamic_environment.h"
#include "dynamic_sensors.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CoreDynamic 传感器环境尾区，另保留原函数同时发布的 GCI 磁场。 */
typedef struct {
    DpDeviceMeasureEnvironment device_environment;
    DpVec3 magnetic_gci;
} DpCoreEnvironmentOutputs;

/*
 * 使用 CoreDynamic 调用点所见的预积分状态计算惯性磁场，再以积分后状态的姿态
 * 将太阳和磁场投影到机体系；calendar_time 必须已完成该步 TimeAdd。
 */
int dp_core_environment_from_states(DpCoreEnvironmentOutputs *out,
                                    const double pre_integration_state[DP_STATE_DIM],
                                    const double post_integration_state[DP_STATE_DIM],
                                    const DpCalendarTime *calendar_time);

/* 默认 RK4 桥接与上述环境装配的组合接口。 */
int dp_core_default_rk4_step_with_terms_and_environment(
    DpCoreDefaultModel *model, double state[DP_STATE_DIM], double *time, double step,
    const DpVec3 *angular_momentum, const DpVec3 *minus_term,
    DpCoreEnvironmentOutputs *out_environment);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_CORE_ENVIRONMENT_H */

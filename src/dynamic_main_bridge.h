#ifndef DYNAMIC_MAIN_BRIDGE_H
#define DYNAMIC_MAIN_BRIDGE_H

#include "dynamic_core_environment.h"
#include "dynamic_devices.h"
#include "dynamic_ipc_telemetry.h"
#include "dynamic_telemetry_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    DpCoreDefaultModel *core_model;
    DpDeviceMeasureRecovered *devices;
    DpDeviceControlContext *device_control;
    double *integration_time;
    double step_time;
    /* 非正值时兼容旧调用，回退使用 step_time。 */
    double device_control_step_time;
    DpGaussian2Sampler gaussian2_sampler;
    void *gaussian2_opaque;
    /* 原 MagUpdate 在 CoreDynamic 后刷新，下一逻辑步的 MagTorque 使用该缓存。 */
    DpVec3 prior_magnetic_body;
    uint32_t prior_magnetic_body_valid;
} DpDynMainRecoveredContext;

/* 与原 UpdateCoreDynInput / UpdateTorque 最终输入对应的已组装核心项。 */
typedef struct {
    DpVec3 wheel_angular_momentum;
    DpVec3 minus_total_torque;
} DpDynMainCoreTerms;

/* sendDynTele 的五个 DRC 字节输入按原顺序：+0x20,+0x24,+0x40,+0x44,+0x8c。 */
typedef struct {
    uint8_t drc_data_20;
    uint8_t drc_data_24;
    uint8_t drc_data_40;
    uint8_t drc_data_44;
    uint8_t drc_data_8c;
} DpDynMainIpcControl;

/*
 * 恢复端单步入口：设备控制→默认 CoreDynamic/环境→传感器→UpdateMainOut→可选 IPC。
 * 调用方负责按原对象配置初始化设备、映射矩阵和核心项；共享帧为 NULL 时跳过 IPC 发布。
 */
int dp_dyn_main_recovered_step(DpDynMainRecoveredContext *context,
                               DpState *state,
                               const DpDeviceControlCommand *command,
                               const DpDynMainCoreTerms *core_terms,
                               DpMainTelemetryFrame *out_telemetry,
                               DpIpcSharedFrame *optional_ipc_frame,
                               const DpDynMainIpcControl *ipc_control);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_MAIN_BRIDGE_H */

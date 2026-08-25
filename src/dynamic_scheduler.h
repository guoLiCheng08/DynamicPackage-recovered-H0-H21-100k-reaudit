/* DynamicPackage dyn_main 单步调度恢复。 */
#ifndef DYNAMIC_SCHEDULER_H
#define DYNAMIC_SCHEDULER_H

#include "dynamic_core_layout.h"
#include "dynamic_telemetry_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*update_device_control)(const DpStepCommand *command, double scale, void *opaque);
    void (*update_core_input)(DpCoreDynInput *core_input, void *opaque);
    void (*core_dynamic)(DpState *state, const DpCoreDynInput *core_input, void *opaque);
    void (*update_device_measure)(DpState *state, void *opaque);
    void (*update_main_out)(DpMainTelemetryFrame *out, const DpState *state, void *opaque);
    void *opaque;
} DpSchedulerCallbacks;

/* 与 dyn_main 一致：控制 -> 输入打包 -> 核心动力学 -> 测量 -> 遥测。 */
void dp_dyn_main_dispatch(DpMainTelemetryFrame *out, DpState *state,
                          const DpStepCommand *command,
                          const DpSchedulerCallbacks *callbacks,
                          double control_scale);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_SCHEDULER_H */

/* dyn_main 的高保真调用顺序恢复。 */
#include "dynamic_scheduler.h"

#include <string.h>

void dp_dyn_main_dispatch(DpMainTelemetryFrame *out, DpState *state,
                          const DpStepCommand *command,
                          const DpSchedulerCallbacks *callbacks,
                          double control_scale)
{
    DpCoreDynInput core_input;
    memset(&core_input, 0, sizeof(core_input));

    /* 对应 0x4020: UpdateDeviceControl -> UpdateCoreDynInput -> CoreDynamic
     * -> UpdateDeviceMeasure -> UpdateMainOut。 */
    callbacks->update_device_control(command, control_scale, callbacks->opaque);
    callbacks->update_core_input(&core_input, callbacks->opaque);
    callbacks->core_dynamic(state, &core_input, callbacks->opaque);
    callbacks->update_device_measure(state, callbacks->opaque);
    callbacks->update_main_out(out, state, callbacks->opaque);
}

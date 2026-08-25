/* UpdateDeviceMeasure 的高保真调用顺序恢复。 */
#include "dynamic_sensor_schedule.h"

void dp_update_device_measure_dispatch(const DpState *state,
                                       const DpVec3 *sun_body,
                                       const DpVec3 *magnetic_body,
                                       double step_time,
                                       const DpSensorCallbacks *callbacks)
{
    /* 对应 0x39b0：STS 姿态 -> STS 有效性 -> Gyro -> DSS -> DSS 有效性
     * -> MagMeter -> GPS。 */
    callbacks->update_sts_quat(&state->attitude_q, step_time, callbacks->opaque);
    callbacks->update_sts_valid(&state->attitude_q, sun_body, callbacks->opaque);
    callbacks->update_gyro(&state->body_rate, callbacks->opaque);
    callbacks->update_dss(sun_body, callbacks->opaque);
    callbacks->update_dss_valid(&state->attitude_q, callbacks->opaque);
    callbacks->update_magmeter(magnetic_body, callbacks->opaque);
    callbacks->update_gps(&state->position_gci, &state->velocity_gci,
                          step_time, callbacks->opaque);
}

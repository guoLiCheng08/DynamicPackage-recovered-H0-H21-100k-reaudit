/* UpdateDeviceMeasure 传感器更新顺序恢复。 */
#ifndef DYNAMIC_SENSOR_SCHEDULE_H
#define DYNAMIC_SENSOR_SCHEDULE_H

#include "dynamic_recovered.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*update_sts_quat)(const DpQuat *attitude, double step_time, void *opaque);
    void (*update_sts_valid)(const DpQuat *attitude, const DpVec3 *sun_vector, void *opaque);
    void (*update_gyro)(const DpVec3 *body_rate, void *opaque);
    void (*update_dss)(const DpVec3 *sun_body, void *opaque);
    void (*update_dss_valid)(const DpQuat *attitude, void *opaque);
    void (*update_magmeter)(const DpVec3 *magnetic_body, void *opaque);
    void (*update_gps)(const DpVec3 *position_gci, const DpVec3 *velocity_gci,
                       double step_time, void *opaque);
    void *opaque;
} DpSensorCallbacks;

/* `environment_tail` 为 CoreDynamic 生成的状态扩展区；偏移来自 UpdateDeviceMeasure。 */
void dp_update_device_measure_dispatch(const DpState *state,
                                       const DpVec3 *sun_body,
                                       const DpVec3 *magnetic_body,
                                       double step_time,
                                       const DpSensorCallbacks *callbacks);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_SENSOR_SCHEDULE_H */

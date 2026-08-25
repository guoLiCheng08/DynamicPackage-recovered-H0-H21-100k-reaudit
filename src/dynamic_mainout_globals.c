#include "dynamic_devices.h"
#include "dynamic_orbit.h"
#include "dynamic_satellite_globals.h"
#include "dynamic_telemetry_layout.h"
#include "dynamic_time.h"

#include <string.h>

void UpdateMainOut(void *out, const void *core_dynamic_output)
{
    const unsigned char *core = core_dynamic_output;
    DpMainTelemetryFrame *frame = out;
    DpTelemetrySourceSnapshot source;
    DpCalendarTime calendar;
    double position_data[3];
    double velocity_data[3];
    double position_ecef_data[3] = {0.0, 0.0, 0.0};
    double velocity_ecef_data[3] = {0.0, 0.0, 0.0};
    DpVector position = {3, 0, position_data};
    DpVector velocity = {3, 0, velocity_data};
    DpVector position_ecef = {3, 0, position_ecef_data};
    DpVector velocity_ecef = {3, 0, velocity_ecef_data};
    unsigned index;

    if (frame == NULL || core == NULL ||
        dp_telemetry_source_from_devices(&source, &DeviceMeasure) != 0) {
        return;
    }
    memcpy(source.propagated_prefix, core + 0x0c8u, sizeof(source.propagated_prefix));
    memcpy(&calendar, core + 0x0c8u, sizeof(calendar));
    /* 汇编以输出 +0x138/+0x150 的 GPS_Kalman 状态作为坐标转换和根数反算输入。 */
    memcpy(position_data, source.gps_value, sizeof(position_data));
    memcpy(velocity_data, source.gps_value + 3u, sizeof(velocity_data));
    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        source.rw_omega[index] = dp_get_wheel_speed(RWheel, index);
    }
    source.sada_raw[0] = SADA.current_angle[0];
    source.sada_raw[1] = SADA.current_angle[1];
    source.sada_raw[2] = SADA.angular_velocity[0];
    source.sada_raw[3] = SADA.angular_velocity[1];

    GCI2ECEF(&position_ecef, &position, &calendar);
    GCI2ECEF(&velocity_ecef, &velocity, &calendar);
    source.position_ecef.x = position_ecef_data[0];
    source.position_ecef.y = position_ecef_data[1];
    source.position_ecef.z = position_ecef_data[2];
    source.velocity_ecef.x = velocity_ecef_data[0];
    source.velocity_ecef.y = velocity_ecef_data[1];
    source.velocity_ecef.z = velocity_ecef_data[2];
    PosVel2Elements_M(&source.orbit_elements[0], &source.orbit_elements[1],
                      &source.orbit_elements[2], &source.orbit_elements[3],
                      &source.orbit_elements[4], &source.orbit_elements[5],
                      &source.orbit_elements[6], &source.orbit_elements[7],
                      position_data, velocity_data);
    dp_update_main_out_pack(frame, &source);
}

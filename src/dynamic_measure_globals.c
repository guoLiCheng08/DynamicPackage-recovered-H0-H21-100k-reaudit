#include "dynamic_sensors.h"
#include "dynamic_satellite_globals.h"

#include <string.h>

DpDeviceMeasureRecovered DeviceMeasure;

void dp_device_measure_globals_relocate(void)
{
    unsigned index;

    for (index = 0u; index < 2u; ++index) {
        const size_t item = (size_t)index * sizeof(DeviceMeasure.gyro[0]);
        DeviceMeasure.gyro[index].projection_matrix.data =
            (double *)((unsigned char *)DeviceMeasure.gyro + item + 0xc8u);
        DeviceMeasure.gyro[index].measure.data =
            (double *)((unsigned char *)DeviceMeasure.gyro + item + 0x128u);
    }
    for (index = 0u; index < 2u; ++index) {
        const size_t item = (size_t)index * sizeof(DeviceMeasure.magmeter[0]);
        DeviceMeasure.magmeter[index].projection_matrix.data =
            (double *)((unsigned char *)DeviceMeasure.magmeter + item + 0xc0u);
        DeviceMeasure.magmeter[index].measure.data =
            (double *)((unsigned char *)DeviceMeasure.magmeter + item + 0x120u);
    }
    for (index = 0u; index < 3u; ++index) {
        const size_t item = (size_t)index * sizeof(DeviceMeasure.sts[0]);
        DeviceMeasure.sts[index].installation_matrix.data =
            (double *)((unsigned char *)DeviceMeasure.sts + item + 0x78u);
        DeviceMeasure.sts[index].error_quat.xyz.data =
            (double *)((unsigned char *)DeviceMeasure.sts + item + 0x118u);
        DeviceMeasure.sts[index].measure_quat.xyz.data =
            (double *)((unsigned char *)DeviceMeasure.sts + item + 0x158u);
    }
    for (index = 0u; index < 2u; ++index) {
        const size_t item = (size_t)index * sizeof(DeviceMeasure.dss[0]);
        DeviceMeasure.dss[index].projection_matrix.data =
            (double *)((unsigned char *)DeviceMeasure.dss + item + 0xb8u);
        DeviceMeasure.dss[index].measure.data =
            (double *)((unsigned char *)DeviceMeasure.dss + item + 0x120u);
    }
}

void dp_device_measure_globals_reset(void)
{
    memset(&DeviceMeasure, 0, sizeof(DeviceMeasure));
    dp_device_measure_globals_relocate();
}

void UpdateDeviceMeasure(const void *core_dynamic_output)
{
    const unsigned char *core = core_dynamic_output;
    DpState state;
    DpDeviceMeasureEnvironment environment;

    if (core == NULL) {
        return;
    }
    memset(&state, 0, sizeof(state));
    memset(&environment, 0, sizeof(environment));

    /* +0x000..+0x030: q[4]/omega[3]；+0x098/+0x0b0: GCI 位置/速度。 */
    memcpy(&state.attitude_q, core + 0x000u, sizeof(state.attitude_q));
    memcpy(&state.body_rate, core + 0x020u, sizeof(state.body_rate));
    memcpy(&state.position_gci, core + 0x098u, sizeof(state.position_gci));
    memcpy(&state.velocity_gci, core + 0x0b0u, sizeof(state.velocity_gci));
    memcpy(&environment.sun_body, core + 0x038u, sizeof(environment.sun_body));
    memcpy(&environment.sun_gci, core + 0x050u, sizeof(environment.sun_gci));
    memcpy(&environment.magnetic_body, core + 0x068u, sizeof(environment.magnetic_body));
    memcpy(environment.time_values, core + 0x0c8u, sizeof(environment.time_values));

    dp_update_device_measure_recovered(&DeviceMeasure, &state, &environment,
                                       step_time, NULL, NULL);
}

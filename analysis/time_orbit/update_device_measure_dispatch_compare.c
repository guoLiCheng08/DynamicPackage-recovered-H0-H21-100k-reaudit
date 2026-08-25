#include <stdio.h>
#include <string.h>

#include "dynamic_sensor_schedule.h"

typedef struct {
    unsigned codes[7];
    unsigned count;
    int failed;
    const DpState *state;
    const DpVec3 *sun;
    const DpVec3 *magnetic;
    double step_time;
} Probe;

static void record(Probe *probe, unsigned code)
{
    if (probe->count >= 7u) {
        probe->failed = 1;
        return;
    }
    probe->codes[probe->count++] = code;
}

static void sts_quat(const DpQuat *attitude, double step_time, void *opaque)
{
    Probe *probe = opaque;
    if (attitude != &probe->state->attitude_q || step_time != probe->step_time) probe->failed = 1;
    record(probe, 1u);
}
static void sts_valid(const DpQuat *attitude, const DpVec3 *sun, void *opaque)
{
    Probe *probe = opaque;
    if (attitude != &probe->state->attitude_q || sun != probe->sun) probe->failed = 1;
    record(probe, 2u);
}
static void gyro(const DpVec3 *body_rate, void *opaque)
{
    Probe *probe = opaque;
    if (body_rate != &probe->state->body_rate) probe->failed = 1;
    record(probe, 3u);
}
static void dss(const DpVec3 *sun, void *opaque)
{
    Probe *probe = opaque;
    if (sun != probe->sun) probe->failed = 1;
    record(probe, 4u);
}
static void dss_valid(const DpQuat *attitude, void *opaque)
{
    Probe *probe = opaque;
    if (attitude != &probe->state->attitude_q) probe->failed = 1;
    record(probe, 5u);
}
static void magmeter(const DpVec3 *magnetic, void *opaque)
{
    Probe *probe = opaque;
    if (magnetic != probe->magnetic) probe->failed = 1;
    record(probe, 6u);
}
static void gps(const DpVec3 *position, const DpVec3 *velocity, double step_time, void *opaque)
{
    Probe *probe = opaque;
    if (position != &probe->state->position_gci || velocity != &probe->state->velocity_gci ||
        step_time != probe->step_time) probe->failed = 1;
    record(probe, 7u);
}

int main(void)
{
    const unsigned expected[7] = {1u, 2u, 3u, 4u, 5u, 6u, 7u};
    DpState state;
    DpVec3 sun = {1.0, 2.0, 3.0};
    DpVec3 magnetic = {4.0, 5.0, 6.0};
    Probe probe;
    DpSensorCallbacks callbacks;
    unsigned index;

    memset(&state, 0, sizeof(state));
    memset(&probe, 0, sizeof(probe));
    memset(&callbacks, 0, sizeof(callbacks));
    probe.state = &state;
    probe.sun = &sun;
    probe.magnetic = &magnetic;
    probe.step_time = 0.125;
    callbacks.update_sts_quat = sts_quat;
    callbacks.update_sts_valid = sts_valid;
    callbacks.update_gyro = gyro;
    callbacks.update_dss = dss;
    callbacks.update_dss_valid = dss_valid;
    callbacks.update_magmeter = magmeter;
    callbacks.update_gps = gps;
    callbacks.opaque = &probe;
    dp_update_device_measure_dispatch(&state, &sun, &magnetic, probe.step_time, &callbacks);
    if (probe.failed || probe.count != 7u) return 1;
    for (index = 0u; index < 7u; ++index) {
        if (probe.codes[index] != expected[index]) {
            printf("FAIL callback[%u]=%u expected=%u\n", index, probe.codes[index], expected[index]);
            return 1;
        }
    }
    puts("UpdateDeviceMeasure dispatch order/ABI compare: PASS");
    return 0;
}

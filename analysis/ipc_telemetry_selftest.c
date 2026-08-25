#include "../src/dynamic_ipc_telemetry.h"

#include <stdio.h>
#include <string.h>

static float get_f32(const DpIpcSharedFrame *shared, unsigned index)
{
    float value;
    memcpy(&value, shared->raw + DP_IPC_FLOAT_BASE + index * sizeof(value), sizeof(value));
    return value;
}

int main(void)
{
    DpMainTelemetryFrame telemetry = {{0}};
    DpTelemetrySourceSnapshot source = {0};
    DpIpcSharedFrame shared = {{0}};

    source.sts_quat[0][0] = 1.25;
    source.sts_valid[2] = 5;
    source.gyro[0].x = 2.5;
    source.dss_value[3] = 3.5;
    source.dss_valid[1] = 6;
    source.magmeter[1].z = 4.5;
    source.rw_omega[3] = 5.5;
    source.sada_raw[0] = 6.5;
    source.sada_raw[1] = 7.5;
    source.sada_raw[2] = 8.5;
    source.sada_raw[3] = 9.5;
    source.gps_value[5] = 10.5;
    source.gps_flag[0] = 2;
    source.gps_flag[5] = 8;
    source.position_ecef.z = 11.5;
    source.velocity_ecef.y = 12.5;
    source.orbit_elements[6] = 13.5;
    dp_update_main_out_pack(&telemetry, &source);
    dp_send_dyn_tele(&shared, &telemetry, 20, 24, 40, 44, 88);

    if (get_f32(&shared, 12) != 1.25f || shared.raw[DP_IPC_U8_BASE + 5] != 5u) return 1;
    if (get_f32(&shared, 24) != 2.5f || get_f32(&shared, 33) != 3.5f ||
        shared.raw[DP_IPC_U8_BASE + 9] != 6u) return 2;
    if (get_f32(&shared, 39) != 4.5f || get_f32(&shared, 43) != 5.5f) return 3;
    if (get_f32(&shared, 62) != 6.5f || get_f32(&shared, 63) != 7.5f ||
        get_f32(&shared, 219) != 8.5f || get_f32(&shared, 220) != 9.5f) return 4;
    if (get_f32(&shared, 49) != 10.5f || shared.raw[DP_IPC_U8_BASE + 12] != 0x32u ||
        shared.raw[DP_IPC_U8_BASE + 17] != 8u) return 5;
    if (get_f32(&shared, 52) != 11.5f || get_f32(&shared, 54) != 12.5f ||
        get_f32(&shared, 61) != 13.5f) return 6;
    if (shared.raw[DP_IPC_U8_BASE + 6] != 20u || shared.raw[DP_IPC_U8_BASE + 7] != 24u ||
        shared.raw[DP_IPC_U8_BASE + 10] != 40u || shared.raw[DP_IPC_U8_BASE + 11] != 44u ||
        shared.raw[DP_IPC_U8_BASE + 18] != 88u || shared.raw[DP_IPC_U8_BASE + 19] != 88u) return 7;
    puts("dynamic_ipc_telemetry selftest: PASS");
    return 0;
}

#include "../src/dynamic_telemetry_layout.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static double get_f64(const uint8_t *raw, unsigned offset)
{
    double value;
    memcpy(&value, raw + offset, sizeof(value));
    return value;
}

static int32_t get_i32(const uint8_t *raw, unsigned offset)
{
    int32_t value;
    memcpy(&value, raw + offset, sizeof(value));
    return value;
}

int main(void)
{
    DpMainTelemetryFrame output = {{0}};
    DpTelemetrySourceSnapshot source = {0};
    source.sts_quat[2][3] = 123.0;
    source.sts_valid[1] = 7;
    source.gyro[0].x = 11.0;
    source.dss_value[3] = 22.0;
    source.magmeter[1].z = 33.0;
    source.rw_omega[3] = 44.0;
    source.sada_raw[0] = 51.0;
    source.sada_raw[1] = 52.0;
    source.sada_raw[2] = 53.0;
    source.sada_raw[3] = 54.0;
    source.gps_value[5] = 61.0;
    source.gps_flag[4] = 62;
    source.position_ecef.y = 71.0;
    source.velocity_ecef.z = 72.0;
    source.orbit_elements[7] = 81.0;

    dp_update_main_out_pack(&output, &source);
    if (get_f64(output.raw, DP_TM_STS_QUAT_2 + 24u) != 123.0) return 1;
    if (get_i32(output.raw, DP_TM_STS_VALID + 4u) != 7) return 2;
    if (get_f64(output.raw, DP_TM_GYRO_0) != 11.0) return 3;
    if (get_f64(output.raw, DP_TM_DSS_VALUE + 24u) != 22.0) return 4;
    if (get_f64(output.raw, DP_TM_MAGMETER_1 + 16u) != 33.0) return 5;
    if (get_f64(output.raw, DP_TM_RW_OMEGA + 24u) != 44.0) return 6;
    if (get_f64(output.raw, DP_TM_SADA + 0u) != 51.0 ||
        get_f64(output.raw, DP_TM_SADA + 8u) != 52.0 ||
        get_f64(output.raw, DP_TM_SADA + 16u) != 53.0 ||
        get_f64(output.raw, DP_TM_SADA + 24u) != 54.0) return 7;
    if (get_f64(output.raw, DP_TM_GPS_VALUE + 40u) != 61.0 ||
        get_i32(output.raw, DP_TM_GPS_FLAG + 16u) != 62) return 8;
    if (get_f64(output.raw, DP_TM_POSITION_ECEF + 8u) != 71.0 ||
        get_f64(output.raw, DP_TM_VELOCITY_ECEF + 16u) != 72.0 ||
        get_f64(output.raw, DP_TM_ORBIT_ELEMENTS + 56u) != 81.0) return 9;
    puts("dynamic_telemetry selftest: PASS");
    return 0;
}

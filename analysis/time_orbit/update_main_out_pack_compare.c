#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_telemetry_layout.h"

static double get_f64(const uint8_t *raw, unsigned offset)
{
    double result;
    memcpy(&result, raw + offset, sizeof(result));
    return result;
}

static int32_t get_i32(const uint8_t *raw, unsigned offset)
{
    int32_t result;
    memcpy(&result, raw + offset, sizeof(result));
    return result;
}

static int require_f64(const char *label, const DpMainTelemetryFrame *frame,
                       unsigned offset, double expected)
{
    const double actual = get_f64(frame->raw, offset);
    if (memcmp(&actual, &expected, sizeof(actual)) != 0) {
        printf("FAIL %s actual=%a expected=%a\n", label, actual, expected);
        return 1;
    }
    return 0;
}

int main(void)
{
    DpTelemetrySourceSnapshot source;
    DpMainTelemetryFrame frame;
    unsigned index;

    memset(&source, 0, sizeof(source));
    memset(&frame, 0xa5, sizeof(frame));
    for (index = 0u; index < 3u; ++index) {
        source.sts_valid[index] = (int32_t)(10u + index);
        source.gyro[index % 2u].x = 20.0;
        source.position_ecef.x = 70.0;
        source.velocity_ecef.x = 80.0;
        source.magmeter[index % 2u].x = 40.0;
    }
    for (index = 0u; index < 3u; ++index)
        for (unsigned element = 0u; element < 4u; ++element)
            source.sts_quat[index][element] = 100.0 + (double)(index * 4u + element);
    source.gyro[0] = (DpVec3){20.0, 21.0, 22.0};
    source.gyro[1] = (DpVec3){23.0, 24.0, 25.0};
    for (index = 0u; index < 4u; ++index) {
        source.dss_value[index] = 30.0 + (double)index;
        source.rw_omega[index] = 50.0 + (double)index;
        source.sada_raw[index] = 60.0 + (double)index;
    }
    source.dss_valid[0] = 11; source.dss_valid[1] = 12;
    source.magmeter[0] = (DpVec3){40.0, 41.0, 42.0};
    source.magmeter[1] = (DpVec3){43.0, 44.0, 45.0};
    for (index = 0u; index < 6u; ++index) {
        source.gps_value[index] = 90.0 + (double)index;
        source.gps_flag[index] = (int32_t)(20u + index);
        source.propagated_prefix[index] = 1000.0 + (double)index;
        source.orbit_elements[index] = 110.0 + (double)index;
    }
    source.position_ecef = (DpVec3){70.0, 71.0, 72.0};
    source.velocity_ecef = (DpVec3){80.0, 81.0, 82.0};
    dp_update_main_out_pack(&frame, &source);

    for (index = 0u; index < 3u; ++index) {
        if (get_i32(frame.raw, DP_TM_STS_VALID + index * 4u) != (int32_t)(10u + index)) return 1;
        for (unsigned element = 0u; element < 4u; ++element)
            if (require_f64("sts", &frame, DP_TM_STS_QUAT_0 + index * 0x20u + element * 8u,
                            100.0 + (double)(index * 4u + element))) return 1;
    }
    if (require_f64("gyro0x", &frame, DP_TM_GYRO_0, 20.0) ||
        require_f64("gyro1z", &frame, DP_TM_GYRO_1 + 16u, 25.0) ||
        require_f64("dss3", &frame, DP_TM_DSS_VALUE + 24u, 33.0) ||
        get_i32(frame.raw, DP_TM_DSS_VALID) != 11 ||
        get_i32(frame.raw, DP_TM_DSS_VALID + 4u) != 12 ||
        require_f64("mag1y", &frame, DP_TM_MAGMETER_1 + 8u, 44.0) ||
        require_f64("wheel3", &frame, DP_TM_RW_OMEGA + 24u, 53.0) ||
        require_f64("sada0", &frame, DP_TM_SADA, 60.0) ||
        require_f64("sada1", &frame, DP_TM_SADA + 8u, 61.0) ||
        require_f64("sada2", &frame, DP_TM_SADA + 16u, 62.0) ||
        require_f64("sada3", &frame, DP_TM_SADA + 24u, 63.0) ||
        require_f64("gps5", &frame, DP_TM_GPS_VALUE + 40u, 95.0) ||
        get_i32(frame.raw, DP_TM_GPS_FLAG + 20u) != 25 ||
        require_f64("prefix5", &frame, DP_TM_PROPAGATED_PREFIX + 40u, 1005.0) ||
        require_f64("positionz", &frame, DP_TM_POSITION_ECEF + 16u, 72.0) ||
        require_f64("velocityz", &frame, DP_TM_VELOCITY_ECEF + 16u, 82.0) ||
        require_f64("orbit5", &frame, DP_TM_ORBIT_ELEMENTS + 40u, 115.0)) return 1;
    puts("UpdateMainOut pack layout compare: PASS");
    return 0;
}

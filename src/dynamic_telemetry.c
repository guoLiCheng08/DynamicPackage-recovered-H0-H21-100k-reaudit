/* UpdateMainOut 确定性封包部分的高保真恢复。 */
#include "dynamic_telemetry_layout.h"
#include "dynamic_rng.h"

#include <string.h>

static void put_f64(uint8_t *raw, unsigned offset, double value)
{
    memcpy(raw + offset, &value, sizeof(value));
}

static void put_i32(uint8_t *raw, unsigned offset, int32_t value)
{
    memcpy(raw + offset, &value, sizeof(value));
}

static void put_vec3(uint8_t *raw, unsigned offset, const DpVec3 *value)
{
    memcpy(raw + offset, value, sizeof(*value));
}

void dp_update_main_out_pack(DpMainTelemetryFrame *out,
                             const DpTelemetrySourceSnapshot *source)
{
    unsigned index;

    /* GetStarTrackerQuat 的目标依次为 +0x000/+0x020/+0x040。 */
    for (index = 0; index < 3u; ++index) {
        memcpy(out->raw + DP_TM_STS_QUAT_0 + index * 0x20u,
               source->sts_quat[index], sizeof(source->sts_quat[index]));
        put_i32(out->raw, DP_TM_STS_VALID + index * 4u, source->sts_valid[index]);
    }

    put_vec3(out->raw, DP_TM_GYRO_0, &source->gyro[0]);
    put_vec3(out->raw, DP_TM_GYRO_1, &source->gyro[1]);
    for (index = 0; index < 4u; ++index) {
        put_f64(out->raw, DP_TM_DSS_VALUE + index * 8u, source->dss_value[index]);
    }
    put_i32(out->raw, DP_TM_DSS_VALID + 0u, source->dss_valid[0]);
    put_i32(out->raw, DP_TM_DSS_VALID + 4u, source->dss_valid[1]);
    put_vec3(out->raw, DP_TM_MAGMETER_0, &source->magmeter[0]);
    put_vec3(out->raw, DP_TM_MAGMETER_1, &source->magmeter[1]);
    for (index = 0; index < 4u; ++index) {
        put_f64(out->raw, DP_TM_RW_OMEGA + index * 8u, source->rw_omega[index]);
    }

    /* 原写入顺序：raw[0], raw[2], raw[1], raw[3] -> +118/+120/+128/+130。 */
    put_f64(out->raw, DP_TM_SADA + 0x00u, source->sada_raw[0]);
    put_f64(out->raw, DP_TM_SADA + 0x10u, source->sada_raw[2]);
    put_f64(out->raw, DP_TM_SADA + 0x08u, source->sada_raw[1]);
    put_f64(out->raw, DP_TM_SADA + 0x18u, source->sada_raw[3]);

    for (index = 0; index < 6u; ++index) {
        put_f64(out->raw, DP_TM_GPS_VALUE + index * 8u, source->gps_value[index]);
        put_i32(out->raw, DP_TM_GPS_FLAG + index * 4u, source->gps_flag[index]);
    }
    for (index = 0; index < 6u; ++index) {
        put_f64(out->raw, DP_TM_PROPAGATED_PREFIX + index * 8u,
                source->propagated_prefix[index]);
    }
    put_vec3(out->raw, DP_TM_POSITION_ECEF, &source->position_ecef);
    put_vec3(out->raw, DP_TM_VELOCITY_ECEF, &source->velocity_ecef);
    for (index = 0; index < 8u; ++index) {
        put_f64(out->raw, DP_TM_ORBIT_ELEMENTS + index * 8u, source->orbit_elements[index]);
    }
}

static double dp_f64_from_bits(uint64_t bits)
{
    double value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

int dp_telemetry_source_from_devices(DpTelemetrySourceSnapshot *source,
                                     const DpDeviceMeasureRecovered *devices)
{
    static const uint64_t degree_per_radian_bits = UINT64_C(0x404ca5dc1a63c1f8);
    const double degree_per_radian = dp_f64_from_bits(degree_per_radian_bits);
    unsigned index;

    if (source == NULL || devices == NULL) {
        return -1;
    }
    memset(source, 0, sizeof(*source));
    dp_rng_trace_stage("star_tracker.begin");
    for (index = 0u; index < 3u; ++index) {
        if (dp_get_star_tracker_quat(source->sts_quat[index], &devices->sts[index],
                                     NULL, NULL) != 0) {
            return -1;
        }
        source->sts_valid[index] = devices->sts[index].valid_flag;
    }
    dp_rng_trace_stage("star_tracker.end");
    for (index = 0u; index < 2u; ++index) {
        if (devices->gyro[index].measure.data == NULL ||
            devices->magmeter[index].measure.data == NULL) {
            return -1;
        }
        source->gyro[index].x = devices->gyro[index].measure.data[0];
        source->gyro[index].y = devices->gyro[index].measure.data[1];
        source->gyro[index].z = devices->gyro[index].measure.data[2];
        source->magmeter[index].x = devices->magmeter[index].measure.data[0];
        source->magmeter[index].y = devices->magmeter[index].measure.data[1];
        source->magmeter[index].z = devices->magmeter[index].measure.data[2];
        source->dss_valid[index] = devices->dss[index].valid_flag;
    }
    /* UpdateMainOut: DSS[0].angle_x/angle_y，再 DSS[1].angle_x/angle_y，均乘同一常量。 */
    source->dss_value[0] = devices->dss[0].angle_x * degree_per_radian;
    source->dss_value[1] = devices->dss[0].angle_y * degree_per_radian;
    source->dss_value[2] = devices->dss[1].angle_x * degree_per_radian;
    source->dss_value[3] = devices->dss[1].angle_y * degree_per_radian;
    for (index = 0u; index < 3u; ++index) {
        source->gps_value[index] = devices->gps.position_gci[index];
        source->gps_value[index + 3u] = devices->gps.velocity_gci[index];
    }
    for (index = 0u; index < 6u; ++index) {
        source->gps_flag[index] = devices->gps.time_components[index];
    }
    return 0;
}

/* DynamicPackage UpdateMainOut 遥测帧布局：目标对象符号 `output` 的大小为 0x220。 */
#ifndef DYNAMIC_TELEMETRY_LAYOUT_H
#define DYNAMIC_TELEMETRY_LAYOUT_H

#include <stdint.h>
#include "dynamic_recovered.h"
#include "dynamic_sensors.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DP_MAIN_TELEMETRY_BYTES 0x220u

typedef struct {
    uint8_t raw[DP_MAIN_TELEMETRY_BYTES];
} DpMainTelemetryFrame;

/* 已由 UpdateMainOut 指令确认的输出偏移。 */
enum {
    DP_TM_STS_QUAT_0 = 0x000,
    DP_TM_STS_QUAT_1 = 0x020,
    DP_TM_STS_QUAT_2 = 0x040,
    DP_TM_STS_VALID = 0x060,
    DP_TM_GYRO_0 = 0x070,
    DP_TM_GYRO_1 = 0x088,
    DP_TM_DSS_VALUE = 0x0a0,
    DP_TM_DSS_VALID = 0x0c0,
    DP_TM_MAGMETER_0 = 0x0c8,
    DP_TM_MAGMETER_1 = 0x0e0,
    DP_TM_RW_OMEGA = 0x0f8,
    DP_TM_SADA = 0x118,
    DP_TM_GPS_VALUE = 0x138,
    DP_TM_GPS_FLAG = 0x168,
    DP_TM_PROPAGATED_PREFIX = 0x180,
    DP_TM_POSITION_ECEF = 0x1b0,
    DP_TM_VELOCITY_ECEF = 0x1c8,
    DP_TM_ORBIT_ELEMENTS = 0x1e0
};

/*
 * 传感器数学和设备状态的完整全局布局仍在逆向；本桥接快照表示已经得到的
 * 物理量，并仅处理 UpdateMainOut 的确定性封包顺序和字段重排。
 */
typedef struct {
    double sts_quat[3][4];
    int32_t sts_valid[3];
    DpVec3 gyro[2];
    double dss_value[4];
    int32_t dss_valid[2];
    DpVec3 magmeter[2];
    double rw_omega[4];
    double sada_raw[4];
    double gps_value[6];
    int32_t gps_flag[6];
    double propagated_prefix[6];
    DpVec3 position_ecef;
    DpVec3 velocity_ecef;
    double orbit_elements[8];
} DpTelemetrySourceSnapshot;

void dp_update_main_out_pack(DpMainTelemetryFrame *out,
                             const DpTelemetrySourceSnapshot *source);

/* 从原 UpdateDeviceMeasure 已更新的对象抽取 UpdateMainOut 的传感器／GPS区域。 */
int dp_telemetry_source_from_devices(DpTelemetrySourceSnapshot *source,
                                     const DpDeviceMeasureRecovered *devices);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_TELEMETRY_LAYOUT_H */

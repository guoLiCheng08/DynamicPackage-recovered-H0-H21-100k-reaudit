#ifndef C_SHADOW_STATE_IPC_H
#define C_SHADOW_STATE_IPC_H

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "dynamic_devices.h"
#include "dynamic_telemetry_layout.h"
#include "dynamic_ipc_telemetry.h"

#define DP_C_SHADOW_STATE_SHM "/cfs_test_c_shadow_state"
#define DP_C_SHADOW_STATE_SHM_ENV "C_SHADOW_STATE_SHM"
#define DP_C_SHADOW_STATE_MAGIC UINT32_C(0x43534844)
#define DP_C_SHADOW_STATE_VERSION 8u
#define DP_DEVICE_GLOBAL_SNAPSHOT_BYTES 0x1640u
#define DP_C_SHADOW_SEED_READY UINT32_C(0x53454544)
#define DP_IPC_PAYLOAD_BYTES (DP_IPC_SHM_BYTES - DP_IPC_RWLOCK_BYTES)

static inline const char *dp_c_shadow_state_shm_name(void)
{
    const char *name = getenv(DP_C_SHADOW_STATE_SHM_ENV);
    return name != NULL && name[0] == '/' ? name : DP_C_SHADOW_STATE_SHM;
}

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t sequence;
    uint32_t input_sequence;
    uint32_t state_bytes;
    uint32_t telemetry_bytes;
    uint32_t devices_bytes;
    uint32_t ipc_bytes;
    uint32_t seed_ready;
    uint32_t seed_ack;
    uint32_t post_ack;
    uint32_t reserved;
    /* 测试采样器在 dyn_main 入口读取的实际控制结构；只作为双路共同输入，
     * 不承载 ELF 状态、设备或遥测输出。 */
    DpDeviceControlCommand applied_command;
    double seed_integration_time;
    double seed_time_second_decimal;
    double seed_time_second_total;
    double seed_calendar[6];
    struct tm seed_calendar_tm;
    double integration_time;
    double time_second_decimal;
    double time_second_total;
    double calendar[6];
    struct tm calendar_tm;
    double state[DP_STATE_DIM];
    DpMainTelemetryFrame telemetry;
    DpDeviceMeasureRecovered devices;
    uint8_t seed_device_globals[DP_DEVICE_GLOBAL_SNAPSHOT_BYTES];
    uint8_t device_globals[DP_DEVICE_GLOBAL_SNAPSHOT_BYTES];
    uint8_t ipc_payload[DP_IPC_PAYLOAD_BYTES];
} DpCShadowStateFrame;

#endif

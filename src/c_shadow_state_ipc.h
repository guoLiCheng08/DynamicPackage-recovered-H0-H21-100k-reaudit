#ifndef C_SHADOW_STATE_IPC_H
#define C_SHADOW_STATE_IPC_H

#include <stdint.h>
#include <time.h>

#include "dynamic_devices.h"
#include "dynamic_telemetry_layout.h"
#include "dynamic_ipc_telemetry.h"

#define DP_C_SHADOW_STATE_SHM "/cfs_test_c_shadow_state"
#define DP_C_SHADOW_STATE_MAGIC UINT32_C(0x43534844)
#define DP_C_SHADOW_STATE_VERSION 7u
#define DP_DEVICE_GLOBAL_SNAPSHOT_BYTES 0x1640u
#define DP_C_SHADOW_SEED_READY UINT32_C(0x53454544)
#define DP_IPC_PAYLOAD_BYTES (DP_IPC_SHM_BYTES - DP_IPC_RWLOCK_BYTES)

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

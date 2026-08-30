#ifndef SHADOW_REPLAY_IPC_H
#define SHADOW_REPLAY_IPC_H

#include <stdint.h>
#include <stdlib.h>

#include "dynamic_devices.h"

#define DP_SHADOW_INPUT_SHM "/cfs_test_shadow_input"
#define DP_SHADOW_INPUT_MAGIC UINT32_C(0x43534950)
#define DP_SHADOW_INPUT_VERSION 1u

static inline const char *dp_shadow_input_shm_name(void)
{
    const char *name = getenv("C_SHADOW_INPUT_SHM");
    return name != NULL && name[0] == '/' ? name : DP_SHADOW_INPUT_SHM;
}

/* 单生产者、单消费者的输入回放帧。sequence=0 表示尚未发布；发布者最后
 * 写 sequence，消费者只接受大于上次消费序号的完整帧。 */
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t sequence;
    uint32_t reserved;
    double step_time;
    DpDeviceControlCommand command;
} DpShadowReplayFrame;

#endif

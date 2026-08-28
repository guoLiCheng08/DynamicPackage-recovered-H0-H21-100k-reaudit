#define _POSIX_C_SOURCE 200809L

#include "c_shadow_state_ipc.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define ELF_STATE_SHM "/cfs_test_elf_state"

static int map_frame(const char *name, int *fd_out, DpCShadowStateFrame **frame_out)
{
    int fd = shm_open(name, O_RDONLY, 0);
    DpCShadowStateFrame *frame;
    if (fd < 0) return -1;
    frame = mmap(NULL, sizeof(*frame), PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (frame == MAP_FAILED) return -1;
    *fd_out = -1;
    *frame_out = frame;
    return 0;
}

int main(void)
{
    const char *c_name = getenv("C_SHADOW_STATE_SHM");
    DpCShadowStateFrame *c_frame = NULL;
    DpCShadowStateFrame *elf_frame = NULL;
    unsigned index;
    unsigned mismatches = 0u;
    uint32_t input_sequence;
    unsigned telemetry_mismatches = 0u;
    unsigned telemetry_diff_bytes = 0u;
    unsigned device_mismatches = 0u;
    unsigned ipc_mismatches = 0u;
    unsigned ipc_diff_bytes = 0u;
    int unused_fd;
    static const struct {
        const char *name;
        unsigned offset;
        unsigned bytes;
    } telemetry_regions[] = {
        {"星敏感器四元数", 0x000u, 0x060u},
        {"星敏感器状态", 0x060u, 0x00cu},
        {"陀螺", 0x070u, 0x030u},
        {"太阳敏感器", 0x0a0u, 0x028u},
        {"磁强计", 0x0c8u, 0x030u},
        {"反作用轮", 0x0f8u, 0x020u},
        {"SADA", 0x118u, 0x020u},
        {"GPS", 0x138u, 0x048u},
        {"传播时间", 0x180u, 0x030u},
        {"ECEF", 0x1b0u, 0x030u},
        {"轨道根数", 0x1e0u, 0x040u}
    };
    unsigned region_index;

    if (c_name == NULL || c_name[0] != '/') c_name = DP_C_SHADOW_STATE_SHM;
    if (map_frame(c_name, &unused_fd, &c_frame) != 0 ||
        map_frame(ELF_STATE_SHM, &unused_fd, &elf_frame) != 0) {
        fprintf(stderr, "无法打开 C/ELF 状态帧\n");
        return 2;
    }
    if (c_frame->magic != DP_C_SHADOW_STATE_MAGIC ||
        elf_frame->magic != DP_C_SHADOW_STATE_MAGIC ||
        c_frame->version != DP_C_SHADOW_STATE_VERSION ||
        elf_frame->version != DP_C_SHADOW_STATE_VERSION ||
        c_frame->input_sequence == 0u ||
        c_frame->input_sequence != elf_frame->input_sequence) {
        printf("1. 结果=失败\n2. 原因=C/ELF输入序号不一致或状态帧无效\n");
        printf("3. C输入序号=%u\n4. ELF输入序号=%u\n",
               c_frame->input_sequence, elf_frame->input_sequence);
        munmap(c_frame, sizeof(*c_frame));
        munmap(elf_frame, sizeof(*elf_frame));
        return 1;
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        if (memcmp(&c_frame->state[index], &elf_frame->state[index],
                   sizeof(c_frame->state[index])) != 0) {
            printf("状态[%u] C=%.17g ELF=%.17g\n", index,
                   c_frame->state[index], elf_frame->state[index]);
            ++mismatches;
        }
    }
    if (c_frame->telemetry_bytes == sizeof(c_frame->telemetry) &&
        elf_frame->telemetry_bytes == sizeof(elf_frame->telemetry) &&
        memcmp(&c_frame->telemetry, &elf_frame->telemetry,
               sizeof(c_frame->telemetry)) != 0) {
        telemetry_mismatches = 1u;
        for (index = 0u; index < sizeof(c_frame->telemetry); ++index) {
            const uint8_t *c_bytes = c_frame->telemetry.raw;
            const uint8_t *elf_bytes = elf_frame->telemetry.raw;
            if (c_bytes[index] != elf_bytes[index]) {
                if (telemetry_diff_bytes == 0u)
                    printf("首个主遥测差异：raw+0x%03x C=0x%02x ELF=0x%02x\n",
                           index, c_bytes[index], elf_bytes[index]);
                ++telemetry_diff_bytes;
            }
        }
        for (region_index = 0u;
             region_index < sizeof(telemetry_regions) / sizeof(telemetry_regions[0]);
             ++region_index) {
            unsigned region_diff = 0u;
            for (index = telemetry_regions[region_index].offset;
                 index < telemetry_regions[region_index].offset + telemetry_regions[region_index].bytes;
                 ++index) {
                if (c_frame->telemetry.raw[index] != elf_frame->telemetry.raw[index])
                    ++region_diff;
            }
            if (region_diff != 0u)
                printf("主遥测字段差异：%s=%u字节\n",
                       telemetry_regions[region_index].name, region_diff);
        }
    }
    if (c_frame->devices_bytes == DP_DEVICE_GLOBAL_SNAPSHOT_BYTES &&
        elf_frame->devices_bytes == DP_DEVICE_GLOBAL_SNAPSHOT_BYTES &&
        memcmp(c_frame->device_globals, elf_frame->device_globals,
               DP_DEVICE_GLOBAL_SNAPSHOT_BYTES) != 0) {
        device_mismatches = 1u;
    }
    if (c_frame->ipc_bytes == DP_IPC_PAYLOAD_BYTES &&
        elf_frame->ipc_bytes == DP_IPC_PAYLOAD_BYTES &&
        memcmp(c_frame->ipc_payload, elf_frame->ipc_payload,
               DP_IPC_PAYLOAD_BYTES) != 0) {
        ipc_mismatches = 1u;
        for (index = 0u; index < DP_IPC_PAYLOAD_BYTES; ++index) {
            if (c_frame->ipc_payload[index] != elf_frame->ipc_payload[index]) {
                if (ipc_diff_bytes == 0u)
                    printf("首个IPC差异：payload+0x%03x C=0x%02x ELF=0x%02x\n",
                           index, c_frame->ipc_payload[index], elf_frame->ipc_payload[index]);
                ++ipc_diff_bytes;
            }
        }
    }
    input_sequence = c_frame->input_sequence;
    munmap(c_frame, sizeof(*c_frame));
    munmap(elf_frame, sizeof(*elf_frame));
    if (mismatches != 0u || telemetry_mismatches != 0u || device_mismatches != 0u || ipc_mismatches != 0u) {
        printf("1. 结果=失败\n2. 输入序号=%u\n3. 状态不一致项=%u/%u\n4. 主遥测不一致=%s（差异字节=%u）\n5. 设备全局快照不一致=%s\n6. 全部IPC输出不一致=%s（差异字节=%u）\n",
               input_sequence, mismatches, DP_STATE_DIM,
               telemetry_mismatches != 0u ? "是" : "否", telemetry_diff_bytes,
               device_mismatches != 0u ? "是" : "否",
               ipc_mismatches != 0u ? "是" : "否", ipc_diff_bytes);
        return 1;
    }
    printf("1. 结果=通过\n2. 输入序号=%u\n3. 33维状态=逐字节一致\n",
           input_sequence);
    return 0;
}

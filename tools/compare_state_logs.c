#define _POSIX_C_SOURCE 200809L

#include "c_shadow_state_ipc.h"

#include <stdio.h>
#include <string.h>

static void clear_pointer(uint8_t *base, unsigned offset)
{
    memset(base + offset, 0, sizeof(void *));
}

/* 设备快照包含同一对象内部 backing 的进程虚拟地址。地址不是模型输出，
 * 逐字段比较前仅归零 descriptor 的 data 指针，保留其余状态和配置字节。 */
static void canonicalize_device_snapshot(uint8_t snapshot[DP_DEVICE_GLOBAL_SNAPSHOT_BYTES])
{
    unsigned index;
    const unsigned gyro_base = 0x450u;
    const unsigned dss_base = 0x0a90u;
    const unsigned mag_base = 0x0d40u;
    const unsigned sada_base = 0x1020u;
    const unsigned wheel_base = 0x1088u;
    const unsigned mtq_base = 0x1268u;
    const unsigned thruster_base = 0x13b8u;
    const unsigned wheel_group_base = 0x1468u;
    const unsigned mtq_group_base = 0x1530u;

    for (index = 0u; index < 3u; ++index) {
        const unsigned base = index * 0x170u;
        clear_pointer(snapshot, base + 0x70u);
        clear_pointer(snapshot, base + 0x108u);
        clear_pointer(snapshot, base + 0x150u);
    }
    for (index = 0u; index < 2u; ++index) {
        const unsigned base = gyro_base + index * 0x320u;
        clear_pointer(snapshot, base + 0x60u);
        clear_pointer(snapshot, base + 0x78u);
        clear_pointer(snapshot, base + 0x120u);
    }
    for (index = 0u; index < 2u; ++index) {
        const unsigned base = dss_base + index * 0x158u;
        clear_pointer(snapshot, base + 0x50u);
        clear_pointer(snapshot, base + 0x68u);
        clear_pointer(snapshot, base + 0x118u);
    }
    for (index = 0u; index < 2u; ++index) {
        const unsigned base = mag_base + index * 0x138u;
        clear_pointer(snapshot, base + 0x58u);
        clear_pointer(snapshot, base + 0x70u);
        clear_pointer(snapshot, base + 0x118u);
    }
    (void)sada_base;
    for (index = 0u; index < 4u; ++index)
        clear_pointer(snapshot, wheel_base + index * 0x78u + 0x58u);
    for (index = 0u; index < 6u; ++index)
        clear_pointer(snapshot, mtq_base + index * 0x38u + 0x18u);
    clear_pointer(snapshot, thruster_base + 0x18u);
    clear_pointer(snapshot, thruster_base + 0x40u);
    clear_pointer(snapshot, thruster_base + 0x68u);
    clear_pointer(snapshot, thruster_base + 0x90u);
    clear_pointer(snapshot, wheel_group_base + 0x08u);
    clear_pointer(snapshot, wheel_group_base + 0x30u);
    clear_pointer(snapshot, wheel_group_base + 0x60u);
    clear_pointer(snapshot, mtq_group_base + 0x08u);
    clear_pointer(snapshot, mtq_group_base + 0x30u);
    clear_pointer(snapshot, mtq_group_base + 0x78u);
}

static int device_snapshot_equal(const uint8_t c_snapshot[DP_DEVICE_GLOBAL_SNAPSHOT_BYTES],
                                 const uint8_t elf_snapshot[DP_DEVICE_GLOBAL_SNAPSHOT_BYTES],
                                 unsigned *first_offset, uint8_t *c_value, uint8_t *elf_value)
{
    uint8_t c_canonical[DP_DEVICE_GLOBAL_SNAPSHOT_BYTES];
    uint8_t elf_canonical[DP_DEVICE_GLOBAL_SNAPSHOT_BYTES];

    memcpy(c_canonical, c_snapshot, sizeof(c_canonical));
    memcpy(elf_canonical, elf_snapshot, sizeof(elf_canonical));
    canonicalize_device_snapshot(c_canonical);
    canonicalize_device_snapshot(elf_canonical);
    for (unsigned index = 0u; index < sizeof(c_canonical); ++index) {
        if (c_canonical[index] != elf_canonical[index]) {
            *first_offset = index;
            *c_value = c_canonical[index];
            *elf_value = elf_canonical[index];
            return 0;
        }
    }
    return 1;
}

static int ipc_output_equal(const uint8_t c_payload[DP_IPC_PAYLOAD_BYTES],
                            const uint8_t elf_payload[DP_IPC_PAYLOAD_BYTES],
                            unsigned *first_offset)
{
    unsigned index;
    const unsigned byte_base = DP_IPC_U8_BASE - DP_IPC_FLOAT_BASE;

    for (index = 3u; index <= 19u; ++index) {
        if (c_payload[byte_base + index] != elf_payload[byte_base + index]) {
            *first_offset = byte_base + index;
            return 0;
        }
    }
    for (index = 12u; index <= 63u; ++index) {
        const unsigned offset = index * sizeof(float);
        if (memcmp(c_payload + offset, elf_payload + offset, sizeof(float)) != 0) {
            *first_offset = offset;
            return 0;
        }
    }
    for (index = 219u; index <= 220u; ++index) {
        const unsigned offset = index * sizeof(float);
        if (memcmp(c_payload + offset, elf_payload + offset, sizeof(float)) != 0) {
            *first_offset = offset;
            return 0;
        }
    }
    return 1;
}

static int compare_frame(const DpCShadowStateFrame *c_frame,
                         const DpCShadowStateFrame *elf_frame,
                         unsigned row, int show_details)
{
    unsigned index;
    unsigned state_mismatches = 0u;
    int telemetry_mismatch = 0;
    int device_mismatch = 0;
    int ipc_mismatch = 0;
    unsigned device_offset = 0u;
    unsigned ipc_offset = 0u;
    uint8_t c_device_value = 0u;
    uint8_t elf_device_value = 0u;

    if (c_frame->input_sequence != elf_frame->input_sequence) {
        printf("行=%u 序号不一致 C=%u ELF=%u\n", row,
               c_frame->input_sequence, elf_frame->input_sequence);
        return 1;
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        if (memcmp(&c_frame->state[index], &elf_frame->state[index],
                   sizeof(c_frame->state[index])) != 0) {
            if (show_details != 0 && state_mismatches == 0u) {
                printf("首个状态分叉：行=%u 序号=%u 状态[%u] C=%.17g ELF=%.17g\n",
                       row, c_frame->input_sequence, index,
                       c_frame->state[index], elf_frame->state[index]);
                printf("积分时间：C=%.17g ELF=%.17g\n",
                       c_frame->integration_time, elf_frame->integration_time);
            }
            ++state_mismatches;
        }
    }
    if (c_frame->telemetry_bytes == sizeof(c_frame->telemetry) &&
        elf_frame->telemetry_bytes == sizeof(elf_frame->telemetry)) {
        telemetry_mismatch = memcmp(&c_frame->telemetry, &elf_frame->telemetry,
                                    sizeof(c_frame->telemetry)) != 0;
    }
    if (c_frame->devices_bytes == DP_DEVICE_GLOBAL_SNAPSHOT_BYTES &&
        elf_frame->devices_bytes == DP_DEVICE_GLOBAL_SNAPSHOT_BYTES) {
        device_mismatch = !device_snapshot_equal(c_frame->device_globals,
                                                 elf_frame->device_globals,
                                                 &device_offset, &c_device_value,
                                                 &elf_device_value);
    }
    if (c_frame->ipc_bytes == DP_IPC_PAYLOAD_BYTES &&
        elf_frame->ipc_bytes == DP_IPC_PAYLOAD_BYTES) {
        ipc_mismatch = !ipc_output_equal(c_frame->ipc_payload, elf_frame->ipc_payload,
                                         &ipc_offset);
    }
    if (state_mismatches != 0u || telemetry_mismatch || device_mismatch || ipc_mismatch) {
        if (show_details != 0) {
            printf("行=%u 序号=%u 结果=失败 状态差异=%u/%u 遥测差异=%s 设备差异=%s IPC差异=%s\n",
                   row, c_frame->input_sequence, state_mismatches, DP_STATE_DIM,
                   telemetry_mismatch ? "是" : "否", device_mismatch ? "是" : "否",
                   ipc_mismatch ? "是" : "否");
        }
        if (show_details != 0 && device_mismatch) {
            printf("首个设备快照分叉：偏移=0x%04x C=0x%02x ELF=0x%02x\n",
                   device_offset, c_device_value, elf_device_value);
        }
        if (show_details != 0 && ipc_mismatch) {
            printf("首个IPC有效载荷分叉：偏移=0x%04x C=0x%02x ELF=0x%02x\n",
                   ipc_offset, c_frame->ipc_payload[ipc_offset],
                   elf_frame->ipc_payload[ipc_offset]);
        }
        return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    FILE *c_log;
    FILE *elf_log;
    DpCShadowStateFrame c_frame;
    DpCShadowStateFrame elf_frame;
    unsigned row = 0u;
    unsigned compared = 0u;
    unsigned failed = 0u;

    if (argc != 3) {
        fprintf(stderr, "用法: %s C状态日志 ELF状态日志\n", argv[0]);
        return 2;
    }
    c_log = fopen(argv[1], "rb");
    elf_log = fopen(argv[2], "rb");
    if (c_log == NULL || elf_log == NULL) {
        fprintf(stderr, "无法打开逐步状态日志\n");
        if (c_log != NULL) fclose(c_log);
        if (elf_log != NULL) fclose(elf_log);
        return 2;
    }
    for (;;) {
        size_t c_read = fread(&c_frame, sizeof(c_frame), 1u, c_log);
        size_t elf_read = fread(&elf_frame, sizeof(elf_frame), 1u, elf_log);
        if (c_read == 0u || elf_read == 0u) {
            if (c_read != elf_read) {
                printf("结果=失败 日志行数不一致 C=%u ELF=%u\n", row + (unsigned)(c_read != 0u), row);
                failed = 1u;
            }
            break;
        }
        ++row;
        if (c_frame.magic != DP_C_SHADOW_STATE_MAGIC ||
            elf_frame.magic != DP_C_SHADOW_STATE_MAGIC ||
            c_frame.version != DP_C_SHADOW_STATE_VERSION ||
            elf_frame.version != DP_C_SHADOW_STATE_VERSION) {
            printf("行=%u 结果=失败 状态帧版本无效\n", row);
            failed = 1u;
            continue;
        }
        ++compared;
        if (compare_frame(&c_frame, &elf_frame, row, failed == 0u) != 0) ++failed;
    }
    fclose(c_log);
    fclose(elf_log);
    printf("1. 结果=%s\n2. 逐步比较=%u行\n3. 失败行=%u\n",
           failed == 0u ? "通过" : "失败", compared, failed);
    return failed == 0u ? 0 : 1;
}

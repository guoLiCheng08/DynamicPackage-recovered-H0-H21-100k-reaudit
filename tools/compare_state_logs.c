#define _POSIX_C_SOURCE 200809L

#include "c_shadow_state_ipc.h"

#include <stdio.h>
#include <string.h>

static int compare_frame(const DpCShadowStateFrame *c_frame,
                         const DpCShadowStateFrame *elf_frame,
                         unsigned row)
{
    unsigned index;
    unsigned state_mismatches = 0u;
    int telemetry_mismatch = 0;
    int device_mismatch = 0;

    if (c_frame->input_sequence != elf_frame->input_sequence) {
        printf("行=%u 序号不一致 C=%u ELF=%u\n", row,
               c_frame->input_sequence, elf_frame->input_sequence);
        return 1;
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        if (memcmp(&c_frame->state[index], &elf_frame->state[index],
                   sizeof(c_frame->state[index])) != 0) {
            if (state_mismatches == 0u) {
                printf("首个状态分叉：行=%u 序号=%u 状态[%u] C=%.17g ELF=%.17g\n",
                       row, c_frame->input_sequence, index,
                       c_frame->state[index], elf_frame->state[index]);
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
        device_mismatch = memcmp(c_frame->device_globals, elf_frame->device_globals,
                                 DP_DEVICE_GLOBAL_SNAPSHOT_BYTES) != 0;
    }
    if (state_mismatches != 0u || telemetry_mismatch || device_mismatch) {
        printf("行=%u 序号=%u 结果=失败 状态差异=%u/%u 遥测差异=%s 设备差异=%s\n",
               row, c_frame->input_sequence, state_mismatches, DP_STATE_DIM,
               telemetry_mismatch ? "是" : "否", device_mismatch ? "是" : "否");
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
        if (compare_frame(&c_frame, &elf_frame, row) != 0) failed = 1u;
    }
    fclose(c_log);
    fclose(elf_log);
    printf("1. 结果=%s\n2. 逐步比较=%u行\n3. 失败行=%u\n",
           failed == 0u ? "通过" : "失败", compared, failed);
    return failed == 0u ? 0 : 1;
}

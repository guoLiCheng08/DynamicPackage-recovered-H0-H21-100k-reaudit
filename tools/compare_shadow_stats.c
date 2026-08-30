#define _POSIX_C_SOURCE 200809L

#include "c_shadow_state_ipc.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static const struct {
    const char *name;
    unsigned offset;
    unsigned bytes;
} telemetry_regions[] = {
    {"星敏感器四元数", 0x000u, 0x060u},
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

int main(int argc, char **argv)
{
    FILE *c_log;
    FILE *elf_log;
    DpCShadowStateFrame c_frame;
    DpCShadowStateFrame elf_frame;
    unsigned row = 0u;
    unsigned sequence_mismatch = 0u;
    unsigned state_mismatch = 0u;
    unsigned telemetry_diff[sizeof(telemetry_regions) / sizeof(telemetry_regions[0])] = {0u};
    double max_state_error = 0.0;
    unsigned max_state_row = 0u;
    unsigned max_state_index = 0u;
    unsigned first_rng_mismatch_row = 0u;

    if (argc != 3) {
        fprintf(stderr, "用法: %s C状态日志 ELF状态日志\n", argv[0]);
        return 2;
    }
    c_log = fopen(argv[1], "rb");
    elf_log = fopen(argv[2], "rb");
    if (c_log == NULL || elf_log == NULL) return 2;
    while (fread(&c_frame, sizeof(c_frame), 1u, c_log) == 1u &&
           fread(&elf_frame, sizeof(elf_frame), 1u, elf_log) == 1u) {
        unsigned index;
        ++row;
        if (c_frame.input_sequence != elf_frame.input_sequence) ++sequence_mismatch;
        if (c_frame.reserved != elf_frame.reserved && first_rng_mismatch_row == 0u)
            first_rng_mismatch_row = row;
        for (index = 0u; index < DP_STATE_DIM; ++index) {
            double error = fabs(c_frame.state[index] - elf_frame.state[index]);
            if (error > max_state_error) {
                max_state_error = error;
                max_state_row = row;
                max_state_index = index;
            }
            if (memcmp(&c_frame.state[index], &elf_frame.state[index], sizeof(double)) != 0)
                ++state_mismatch;
        }
        for (index = 0u; index < sizeof(telemetry_regions) / sizeof(telemetry_regions[0]); ++index) {
            unsigned byte;
            for (byte = telemetry_regions[index].offset;
                 byte < telemetry_regions[index].offset + telemetry_regions[index].bytes; ++byte) {
                if (c_frame.telemetry.raw[byte] != elf_frame.telemetry.raw[byte])
                    ++telemetry_diff[index];
            }
        }
    }
    fclose(c_log);
    fclose(elf_log);
    printf("1. 结果=统计完成\n2. 按行配对=%u\n3. 输入序号错位=%u\n"
           "4. 状态逐字节差异=%u\n5. 状态最大绝对误差=%.17g（行=%u，状态[%u]）\n",
           row, sequence_mismatch, state_mismatch, max_state_error,
           max_state_row, max_state_index);
    printf("6. 随机数计数首个差异行=%u\n", first_rng_mismatch_row);
    for (unsigned index = 0u; index < sizeof(telemetry_regions) / sizeof(telemetry_regions[0]); ++index)
        printf("6.%u. 主遥测字段=%s，差异字节=%u\n", index + 1u,
               telemetry_regions[index].name, telemetry_diff[index]);
    return 0;
}

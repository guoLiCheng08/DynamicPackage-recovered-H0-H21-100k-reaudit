#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_shadow_state_ipc.h"

static int compare_values(const char *name, const double *c_values,
                          const double *elf_values, size_t count)
{
    size_t index;
    int different = 0;

    for (index = 0u; index < count; ++index) {
        uint64_t c_bits;
        uint64_t elf_bits;

        memcpy(&c_bits, &c_values[index], sizeof(c_bits));
        memcpy(&elf_bits, &elf_values[index], sizeof(elf_bits));
        if (c_bits == elf_bits) {
            continue;
        }
        if (different == 0) {
            printf("字段=%s\n", name);
        }
        printf("  [%zu] C=%.17g (0x%016" PRIx64 ") ELF=%.17g (0x%016" PRIx64 ")\n",
               index, c_values[index], c_bits, elf_values[index], elf_bits);
        different = 1;
    }
    return different;
}

int main(int argc, char **argv)
{
    DpRhsContextTrace c_record;
    DpRhsContextTrace elf_record;
    FILE *c_file;
    FILE *elf_file;
    unsigned records = 0u;
    unsigned failures = 0u;

    if (argc != 3) {
        fprintf(stderr, "用法: %s C上下文.bin ELF上下文.bin\n", argv[0]);
        return 2;
    }
    c_file = fopen(argv[1], "rb");
    elf_file = fopen(argv[2], "rb");
    if (c_file == NULL || elf_file == NULL) {
        fprintf(stderr, "无法打开 RHS 上下文文件\n");
        if (c_file != NULL) (void)fclose(c_file);
        if (elf_file != NULL) (void)fclose(elf_file);
        return 2;
    }
    while (fread(&c_record, sizeof(c_record), 1u, c_file) == 1u &&
           fread(&elf_record, sizeof(elf_record), 1u, elf_file) == 1u) {
        ++records;
        printf("序号=%u 子步=%u\n", c_record.sequence, c_record.stage);
        if (c_record.sequence != elf_record.sequence ||
            c_record.stage != elf_record.stage) {
            printf("字段=序号/子步 C=%u/%u ELF=%u/%u\n", c_record.sequence,
                   c_record.stage, elf_record.sequence, elf_record.stage);
            ++failures;
        }
        failures += (unsigned)compare_values("状态", c_record.state, elf_record.state,
                                             DP_STATE_DIM);
        failures += (unsigned)compare_values("角动量", c_record.angular_momentum,
                                             elf_record.angular_momentum, 3u);
        failures += (unsigned)compare_values("外力矩", c_record.torque,
                                             elf_record.torque, 3u);
        failures += (unsigned)compare_values("惯性磁场", c_record.magnetic_inertial,
                                             elf_record.magnetic_inertial, 3u);
        failures += (unsigned)compare_values("上一拍机体系磁场",
                                             c_record.magnetic_body_prior,
                                             elf_record.magnetic_body_prior, 3u);
        failures += (unsigned)compare_values("惯量", c_record.inertia,
                                             elf_record.inertia, 9u);
        failures += (unsigned)compare_values("SADA指令角", c_record.sada_command_angle,
                                             elf_record.sada_command_angle, 2u);
        failures += (unsigned)compare_values("SADA角加速度",
                                             c_record.sada_angular_acceleration,
                                             elf_record.sada_angular_acceleration, 2u);
        failures += (unsigned)compare_values("柔性耦合矩阵", c_record.coupling,
                                             elf_record.coupling, 30u);
        failures += (unsigned)compare_values("柔性A矩阵", c_record.modal_a,
                                             elf_record.modal_a, 100u);
        failures += (unsigned)compare_values("柔性D矩阵", c_record.modal_d,
                                             elf_record.modal_d, 100u);
        failures += (unsigned)compare_values("SADA动量映射",
                                             c_record.sada_command_momentum_map,
                                             elf_record.sada_command_momentum_map, 9u);
        failures += (unsigned)compare_values("SADA预映射", c_record.sada_modal_pre_map,
                                             elf_record.sada_modal_pre_map, 9u);
        failures += (unsigned)compare_values("SADA模态加速度映射",
                                             c_record.sada_modal_acceleration_map,
                                             elf_record.sada_modal_acceleration_map, 30u);
    }
    if (fread(&c_record, sizeof(c_record), 1u, c_file) == 1u ||
        fread(&elf_record, sizeof(elf_record), 1u, elf_file) == 1u) {
        printf("字段=记录数量 C/ELF不一致\n");
        ++failures;
    }
    (void)fclose(c_file);
    (void)fclose(elf_file);
    printf("1. 采样记录=%u\n2. 字段差异=%u\n3. 结果=%s\n", records, failures,
           failures == 0u && records == 4u ? "通过" : "失败");
    return failures == 0u && records == 4u ? 0 : 1;
}

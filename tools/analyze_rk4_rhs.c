#include "c_shadow_state_ipc.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t bits_of(double value)
{
    uint64_t bits;

    memcpy(&bits, &value, sizeof(bits));
    return bits;
}

static int read_frame(FILE *input, uint32_t sequence, DpCShadowStateFrame *frame)
{
    DpCShadowStateFrame candidate;

    while (fread(&candidate, sizeof(candidate), 1u, input) == 1u) {
        if (candidate.input_sequence == sequence) {
            *frame = candidate;
            return 0;
        }
    }
    return -1;
}

int main(int argc, char **argv)
{
    FILE *c_log;
    FILE *elf_log;
    FILE *rhs_file;
    FILE *c_rhs_file = NULL;
    DpCShadowStateFrame c_previous;
    DpCShadowStateFrame elf_previous;
    DpCShadowStateFrame c_current;
    DpCShadowStateFrame elf_current;
    double rhs[4][DP_STATE_DIM];
    double c_rhs[4][DP_STATE_DIM];
    double weighted;
    double expected;
    uint32_t sequence;
    unsigned index;

    if (argc != 5 && argc != 6) {
        fprintf(stderr, "用法: %s C状态 ELF状态 ELF_RHS [C_RHS] 序号\n", argv[0]);
        return 2;
    }
    sequence = (uint32_t)strtoul(argv[argc - 1], NULL, 10);
    if (sequence == 0u ||
        (c_log = fopen(argv[1], "rb")) == NULL ||
        (elf_log = fopen(argv[2], "rb")) == NULL ||
        (rhs_file = fopen(argv[3], "rb")) == NULL ||
        read_frame(c_log, sequence - 1u, &c_previous) != 0 ||
        read_frame(elf_log, sequence - 1u, &elf_previous) != 0 ||
        read_frame(c_log, sequence, &c_current) != 0 ||
        read_frame(elf_log, sequence, &elf_current) != 0 ||
        fread(rhs, sizeof(rhs), 1u, rhs_file) != 1u) {
        fprintf(stderr, "无法读取RK4分析输入\n");
        return 3;
    }
    (void)fclose(c_log);
    (void)fclose(elf_log);
    (void)fclose(rhs_file);

    if (argc == 6) {
        c_rhs_file = fopen(argv[4], "rb");
        if (c_rhs_file == NULL || fread(c_rhs, sizeof(c_rhs), 1u, c_rhs_file) != 1u) {
            fprintf(stderr, "无法读取C RHS分析输入\n");
            if (c_rhs_file != NULL) (void)fclose(c_rhs_file);
            return 3;
        }
        (void)fclose(c_rhs_file);
    }

    printf("1. 序号=%u\n", sequence);
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        const double h1 = rhs[0][index] * 0.01 + 0.0;
        const double h2 = rhs[1][index] * 0.01 + 0.0;
        const double h3 = rhs[2][index] * 0.01 + 0.0;
        const double h4 = rhs[3][index] * 0.01 + 0.0;

        weighted = h4;
        weighted = (h3 + h3) + weighted;
        weighted = (h2 + h2) + weighted;
        weighted = weighted + h1;
        weighted = weighted * 0.16666666666666666 + 0.0;
        expected = weighted + elf_previous.state[index];
        if (bits_of(expected) != bits_of(c_current.state[index]) ||
            bits_of(expected) != bits_of(elf_current.state[index])) {
            printf("状态[%u] 初态=%016llx C=%016llx ELF=%016llx 复算=%016llx\n",
                   index, (unsigned long long)bits_of(elf_previous.state[index]),
                   (unsigned long long)bits_of(c_current.state[index]),
                   (unsigned long long)bits_of(elf_current.state[index]),
                   (unsigned long long)bits_of(expected));
        }
        if (argc == 6) {
            unsigned stage;

            for (stage = 0u; stage < 4u; ++stage) {
                if (bits_of(c_rhs[stage][index]) != bits_of(rhs[stage][index])) {
                    printf("RHS[%u] 状态[%u] C=%016llx ELF=%016llx\n",
                           stage + 1u, index,
                           (unsigned long long)bits_of(c_rhs[stage][index]),
                           (unsigned long long)bits_of(rhs[stage][index]));
                }
            }
        }
    }
    return 0;
}
